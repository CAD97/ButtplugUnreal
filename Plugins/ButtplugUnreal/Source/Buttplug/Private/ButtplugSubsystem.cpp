// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

#include "ButtplugSubsystem.h"

#include "ButtplugConversions.h"
#include "ButtplugDelegateHelper.h"
#include "ButtplugDevice.h"
#include "ButtplugEvents.h"
#include "ButtplugFeature.h"
#include "ButtplugMessage.h"
#include "Engine/Engine.h"
#include "IWebSocket.h"
#include "Logging/LogMacros.h"
#include "Logging/StructuredLog.h"
#include "WebSocketsModule.h"

class UButtplugSubsystem::FLatentStartAction : public FPendingLatentAction
{
private:
	friend class UButtplugSubsystem;

public:
	FLatentStartAction(const FLatentActionInfo& LatentInfo, TObjectPtr<UButtplugSubsystem> Subsystem, EButtplugClientStartResult& OutResult, FString& OutErrorMessage)
		: FPendingLatentAction()
		, ExecutionFunction(LatentInfo.ExecutionFunction)
		, LinkID(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
		, OwningSubsystem(Subsystem)
		, Result(OutResult)
		, ErrorMessage(OutErrorMessage)
	{
	}

	// FPendingLatentAction implementation
public:
	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		Response.FinishAndTriggerIf(bReady, ExecutionFunction, LinkID, CallbackTarget);
	}

	virtual void NotifyObjectDestroyed()
	{
		check(OwningSubsystem->LatentStartAction == this);
		OwningSubsystem->LatentStartAction = nullptr;
	}

	virtual void NotifyActionAborted()
	{
		check(OwningSubsystem->LatentStartAction == this);
		OwningSubsystem->LatentStartAction = nullptr;
	}

#if WITH_EDITOR
	virtual FString GetDescription() const override
	{
		return TEXT("Connecting to Buttplug server");
	}
#endif

private:
	bool bReady = false;
	FName ExecutionFunction;
	int32 LinkID = INDEX_NONE;
	FWeakObjectPtr CallbackTarget;
	TObjectPtr<UButtplugSubsystem> OwningSubsystem;
	EButtplugClientStartResult& Result;
	FString& ErrorMessage;
};

void UButtplugSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	bInitialized = true;
	ClientName = FApp::GetName();

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance->Implements<IButtplugEvents::UClassType>())
	{
		TScriptInterface<IButtplugEvents> ButtplugEvents = TScriptInterface<IButtplugEvents>(GameInstance);
		BUTTPLUG_AddScriptInterface(OnConnected, ButtplugEvents, &IButtplugEvents::OnButtplugServerConnected);
		BUTTPLUG_AddScriptInterface(OnDisconnected, ButtplugEvents, &IButtplugEvents::OnButtplugServerDisconnected);
		BUTTPLUG_AddScriptInterface(OnScanningFinished, ButtplugEvents, &IButtplugEvents::OnButtplugDeviceScanningFinished);
		BUTTPLUG_AddScriptInterface(OnDeviceAdded, ButtplugEvents, &IButtplugEvents::OnButtplugDeviceAdded);
		BUTTPLUG_AddScriptInterface(OnDeviceRemoved, ButtplugEvents, &IButtplugEvents::OnButtplugDeviceRemoved);
	}
}

void UButtplugSubsystem::Deinitialize()
{
	Reset("Shutting down");
	ClientName.Empty();
	bInitialized = false;
}

bool UButtplugSubsystem::IsConnected() const
{
	return WebSocket.IsValid() && WebSocket->IsConnected();
}

const FString& UButtplugSubsystem::GetClientName() const
{
	return ClientName;
}

const FString& UButtplugSubsystem::GetServerName() const
{
	return ServerName;
}

const FString& UButtplugSubsystem::GetServerAddress() const
{
	return ServerAddress;
}

void UButtplugSubsystem::GetDevices(TArray<UButtplugDevice*>& OutDevices) const
{
	for (const TPair<int32, TObjectPtr<UButtplugDevice>>& DeviceEntry : Devices)
	{
		TObjectPtr<UButtplugDevice> Device = DeviceEntry.Value;
		if (Device->IsConnected())
		{
			OutDevices.Add(Device);
		}
	}
}

void UButtplugSubsystem::GetAllDevices(TArray<UButtplugDevice*>& OutDevices) const
{
	for (const TPair<int32, TObjectPtr<UButtplugDevice>>& Device : Devices)
	{
		OutDevices.Add(Device.Value);
	}
}

void UButtplugSubsystem::StartScanning()
{
	EnqueueMessage(MakeUnique<FButtplugMessage::StartScanning>());
}

void UButtplugSubsystem::StopScanning()
{
	EnqueueMessage(MakeUnique<FButtplugMessage::StopScanning>());
}

void UButtplugSubsystem::AsyncStartClient(FLatentActionInfo LatentInfo, EButtplugClientStartResult& OutResult, FString& OutErrorMessage, const FString& InClientName, const FString& InServerAddress)
{
	FLatentActionManager& LatentActionManager = GetGameInstance()->GetLatentActionManager();
	if (LatentActionManager.FindExistingAction<FLatentStartAction>(LatentInfo.CallbackTarget, LatentInfo.UUID))
	{
		return;
	}

	FLatentStartAction* NewLatentStartAction = new FLatentStartAction(LatentInfo, this, OutResult, OutErrorMessage);
	if (WebSocket.IsValid())
	{
		UE_LOGFMT(LogButtplug, Warning, "Already connected to a Buttplug server but attempted to connect again");
		NewLatentStartAction->Result = EButtplugClientStartResult::ConnectionFailed;
		NewLatentStartAction->ErrorMessage = TEXT("already connected");
		NewLatentStartAction->bReady = true;
		return;
	}
	if (LatentStartAction)
	{
		UE_LOGFMT(LogButtplug, Warning, "Already connecting to a Buttplug server but attempted to connect again");
		NewLatentStartAction->Result = EButtplugClientStartResult::ConnectionFailed;
		NewLatentStartAction->ErrorMessage = TEXT("already connecting");
		NewLatentStartAction->bReady = true;
		return;
	}

	LatentStartAction = NewLatentStartAction;
	LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, LatentStartAction);
	StartClient(InClientName, InServerAddress);
}

void UButtplugSubsystem::StartClient(const FString& InClientName, const FString& InServerAddress)
{
	if (WebSocket.IsValid())
	{
		UE_LOGFMT(LogButtplug, Warning, "Already connected to a Buttplug server but attempted to connect again");
		return;
	}

	FStringFormatNamedArguments Args;
	Args.Add(TEXT("GameName"), FApp::GetProjectName());
	Args.Add(TEXT("AppName"), FApp::GetName());
	Args.Add(TEXT("BuildConfiguration"), LexToString(FApp::GetBuildConfiguration()));

	if (InClientName.IsEmpty())
	{
		static const FName WarningId(TEXT("UButtplugSubsystem::StartClient"));
		FFrame::KismetExecutionMessage(TEXT("No Buttplug client name provided; supplying a default"), ELogVerbosity::Warning, WarningId);
		static const FString DefaultClientNameFormat = TEXT("{AppName} - {GameName} ({BuildConfiguration})");
		ClientName = FString::Format(*DefaultClientNameFormat, Args);
	}
	else
	{
		ClientName = FString::Format(*InClientName, Args);
	}
	ServerAddress = InServerAddress;

	UE_LOGFMT(LogButtplug, Verbose, "Connecting to Buttplug server at {Server} as {Client}", ServerAddress, ClientName);
	WebSocket = FWebSocketsModule::Get().CreateWebSocket(ServerAddress);
	WebSocket->OnConnected().AddUObject(this, &ThisClass::OnSocketConnected);
	WebSocket->OnConnectionError().AddUObject(this, &ThisClass::OnSocketConnectionError);
	WebSocket->OnClosed().AddUObject(this, &ThisClass::OnSocketClosed);
	WebSocket->OnMessage().AddUObject(this, &ThisClass::OnSocketMessage);
	WebSocket->Connect();
}

void UButtplugSubsystem::StopClient()
{
	Reset(TEXT("stopping client"));
}

void UButtplugSubsystem::Tick(float DeltaTime)
{
	if (IsConnected())
	{
		for (const TPair<int32, TObjectPtr<UButtplugDevice>>& DeviceEntry : Devices)
		{
			TObjectPtr<UButtplugDevice> Device = DeviceEntry.Value;
			Device->FlushMessageQueue(DeltaTime);
		}

		if (!MessageBuffer.IsEmpty())
		{
			uint32 FirstId = MessageBuffer[0]->Id;
			UE_LOGFMT(LogButtplug, Verbose, "Sending messages {Min}..{Max} to Buttplug", FirstId, NextMessageId);
			FString Json = WriteButtplugMessagesToJson(MessageBuffer);
			WebSocket->Send(Json);
			MessageBuffer.Empty();
		}
	}
}

ETickableTickType UButtplugSubsystem::GetTickableTickType() const
{
	return ETickableTickType::Always;
}

UWorld* UButtplugSubsystem::GetTickableGameObjectWorld() const
{
	return GetWorld();
}

bool UButtplugSubsystem::IsAllowedToTick() const
{
	return bInitialized;
}

bool UButtplugSubsystem::IsTickableWhenPaused() const
{
	return true;
}

TStatId UButtplugSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UButtplugSubsystem, STATGROUP_Tickables);
}

void UButtplugSubsystem::EnqueueMessage(TUniquePtr<FButtplugMessage> Message)
{
	check(IsConnected());
	Message->Id = NextMessageId++;
	MessageBuffer.Add(MoveTemp(Message));
}

void UButtplugSubsystem::StartPingTimer(float PingRate)
{
	GetGameInstance()->GetTimerManager().SetTimer(PingTimer, this, &ThisClass::TickPingTimer, PingRate, /*bLoop:*/true);
}

void UButtplugSubsystem::TickPingTimer()
{
	if (WebSocket.IsValid())
	{
		EnqueueMessage(MakeUnique<FButtplugMessage::Ping>());
	}
	else
	{
		GetGameInstance()->GetTimerManager().ClearTimer(PingTimer);
	}
}

void UButtplugSubsystem::Reset(const FString& Reason)
{
	GetGameInstance()->GetTimerManager().ClearTimer(PingTimer);

	for (const TPair<int32, TObjectPtr<UButtplugDevice>>& DeviceEntry : Devices)
	{
		DeviceEntry.Value->SetConnected(false);
	}

	if (WebSocket.IsValid())
	{
		int32 CloseCode = 1001; // Going away
		WebSocket->Close(CloseCode, Reason);
		WebSocket = nullptr;
	}

	if (LatentStartAction)
	{
		LatentStartAction->Result = EButtplugClientStartResult::ConnectionFailed;
		LatentStartAction->ErrorMessage = Reason;
		LatentStartAction->bReady = true;
		LatentStartAction = nullptr;
	}

	ClientName.Empty();
	ServerName.Empty();
	ServerAddress.Empty();
	PingTimer.Invalidate();
	NextMessageId = 1;
	MessageBuffer.Empty();
	WebSocket = nullptr;
	LatentStartAction = nullptr;
	// Keep the Devices map around, in case we reconnect.
}

void UButtplugSubsystem::OnSocketConnected()
{
	TUniquePtr<FButtplugMessage::RequestServerInfo> Message = MakeUnique<FButtplugMessage::RequestServerInfo>();
	Message->ClientName = ClientName;
	Message->MessageVersion = FButtplugMessage::SpecVersion();
	EnqueueMessage(MoveTemp(Message));
	// TODO: add timeout for initial handshake
}

void UButtplugSubsystem::OnSocketConnectionError(const FString& Error)
{
	Reset(Error);
}

void UButtplugSubsystem::OnSocketClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	Reset(Reason);
	OnDisconnected.Broadcast();
}

template<EButtplugMessageType MessageType>
void UButtplugSubsystem::OnServerMessage(const TButtplugMessage<MessageType>& Message)
{
	int32 CloseCode = 1008; // Policy violation
	WebSocket->Close(CloseCode, TEXT("server sent a client-to-server message unexpectedly"));
	UE_LOGFMT(LogButtplug, Warning, "Buttplug server sent client message {Message}", Buttplug::Private::GetEnumAsString(Message.GetMessageType()));
}

template<>
void UButtplugSubsystem::OnServerMessage<EButtplugMessageType::Ok>(const FButtplugMessage::Ok& Message)
{
	UE_LOGFMT(LogButtplug, VeryVerbose, "Buttplug server okayed message {Id}", Message.Id);
}

template<>
void UButtplugSubsystem::OnServerMessage<EButtplugMessageType::Error>(const FButtplugMessage::Error& Message)
{
	uint8 ErrorCode = static_cast<uint8>(Message.Code);
	UE_LOGFMT(LogButtplug, Warning, "Buttplug server reported error {Code}; {Message}", ErrorCode, Message.Message);
}

template<>
void UButtplugSubsystem::OnServerMessage<EButtplugMessageType::ServerInfo>(const FButtplugMessage::ServerInfo& Message)
{
	ServerName = Message.ServerName;
	if (Message.MessageVersion == FButtplugMessage::SpecVersion())
	{
		if (LatentStartAction)
		{
			LatentStartAction->Result = EButtplugClientStartResult::Succeeded;
			LatentStartAction->ErrorMessage = TEXT("");
			LatentStartAction->bReady = true;
			LatentStartAction = nullptr;
		}

		UE_LOGFMT(LogButtplug, Verbose, "Connected to Buttplug server {Server} at {Addresss}", ServerName, ServerAddress);
		EnqueueMessage(MakeUnique<FButtplugMessage::RequestDeviceList>());
		// Convert milliseconds to seconds and ping twice as often as required to avoid timeout
		StartPingTimer(Message.MaxPingTime / 2000.0);
		OnConnected.Broadcast();
	}
	else
	{
		int32 CloseCode = 1008; // Policy violation
		WebSocket->Close(CloseCode, TEXT("server responded with incompatible protocol version"));
	}
}

template<>
void UButtplugSubsystem::OnServerMessage<EButtplugMessageType::ScanningFinished>(const FButtplugMessage::ScanningFinished& Message)
{
	OnScanningFinished.Broadcast();
}

template<> // Declare the specialization beforehand so that DeviceList can forward to DeviceAdded
void UButtplugSubsystem::OnServerMessage<EButtplugMessageType::DeviceAdded>(const FButtplugMessage::DeviceAdded& Message);

template<>
void UButtplugSubsystem::OnServerMessage<EButtplugMessageType::DeviceList>(const FButtplugMessage::DeviceList& Message)
{
	// We only ask for a device list on initial connection, and otherwise maintain our own list.
	// Thus a DeviceList message serves to indicate devices present when connecting; add them.
	if (Message.Devices.IsEmpty()) return;
	TUniquePtr<FButtplugMessage::DeviceAdded> SyntheticMessage = MakeUnique<FButtplugMessage::DeviceAdded>();
	SyntheticMessage->Id = Message.Id;
	for (const FButtplugMessage::Device& Device : Message.Devices)
	{
		SyntheticMessage->Device = Device;
		OnServerMessage(*SyntheticMessage);
	}
}

template<>
void UButtplugSubsystem::OnServerMessage<EButtplugMessageType::DeviceAdded>(const FButtplugMessage::DeviceAdded& Message)
{
	TObjectPtr<UButtplugDevice> Device = Devices.FindRef(Message.Device.Index);
	if (Device)
	{
		if (Device->DescriptiveName == Message.Device.Name && Device->DisplayName == Message.Device.DisplayName)
		{
			// If the name matches, we assume the server didn't reuse the device index for a different device.
			// Intiface® Central (the first party server application) has stable indices, even between sessions.
			// Hitting this case for a device with a nonequal set of accepted device messages is very unlikely.
			Device->SetConnected(true);
			OnDeviceAdded.Broadcast(Device);
			return;
		}
		else
		{
			// Device index was reused for a different device. Fallthrough to constructing the new one.
			Device = nullptr;
		}
	}

	Device = Devices.Add(Message.Device.Index, NewObject<UButtplugDevice>(this));
	Device->DeviceIndex = Message.Device.Index;
	Device->DescriptiveName = Message.Device.Name;
	Device->DisplayName = Message.Device.DisplayName;
	Device->DefaultMessageTimingGap = Message.Device.MessageTimingGap / 1000.0; // convert units: (uint)ms to (float)s
	
	// TODO: try to unify features between commands? That's O(n^2) for spec v3 (though feature count is low).
	// I don't think Intiface exposes a device where one feature can be in multiple command arrays yet.

	int32 Index = 0;
	for (const FButtplugMessage::DeviceMessageAttributes& ScalarCmd : Message.Device.Messages.ScalarCmd)
	{
		TObjectPtr<UButtplugFeature> Feature = Device->Features.Add_GetRef(NewObject<UButtplugFeature>(Device));
		Feature->ScalarCmdIndex = Index;
		Feature->FeatureDescriptor = ScalarCmd.FeatureDescriptor;
		Feature->ActuatorStepCount = ScalarCmd.StepCount;
		Feature->FeatureType = Buttplug::Private::GetEnumByName<EButtplugFeatureType>(ScalarCmd.ActuatorType);
		++Index;
	}

	Index = 0;
	for (const FButtplugMessage::DeviceMessageAttributes& LinearCmd : Message.Device.Messages.LinearCmd)
	{
		TObjectPtr<UButtplugFeature> Feature = Device->Features.Add_GetRef(NewObject<UButtplugFeature>(Device));
		Feature->LinearCmdIndex = Index;
		Feature->FeatureDescriptor = LinearCmd.FeatureDescriptor;
		Feature->ActuatorStepCount = LinearCmd.StepCount;
		Feature->FeatureType = Buttplug::Private::GetEnumByName<EButtplugFeatureType>(LinearCmd.ActuatorType);
		++Index;
	}

	Index = 0;
	for (const FButtplugMessage::DeviceMessageAttributes& RotateCmd : Message.Device.Messages.RotateCmd)
	{
		TObjectPtr<UButtplugFeature> Feature = Device->Features.Add_GetRef(NewObject<UButtplugFeature>(Device));
		Feature->RotateCmdIndex = Index;
		Feature->FeatureDescriptor = RotateCmd.FeatureDescriptor;
		Feature->ActuatorStepCount = RotateCmd.StepCount;
		Feature->FeatureType = Buttplug::Private::GetEnumByName<EButtplugFeatureType>(RotateCmd.ActuatorType);
		++Index;
	}

	Index = 0;
	for (const FButtplugMessage::DeviceMessageAttributes& SensorReadCmd : Message.Device.Messages.SensorReadCmd)
	{
		TObjectPtr<UButtplugFeature> Feature = Device->Features.Add_GetRef(NewObject<UButtplugFeature>(Device));
		Feature->SensorReadCmdIndex = Index;
		Feature->FeatureDescriptor = SensorReadCmd.FeatureDescriptor;
		Feature->SensorRange = SensorReadCmd.SensorRange;
		Feature->FeatureType = Buttplug::Private::GetEnumByName<EButtplugFeatureType>(SensorReadCmd.SensorType);
		++Index;
	}

	Index = 0;
	for (const FButtplugMessage::DeviceMessageAttributes& SensorSubscribeCmd : Message.Device.Messages.SensorSubscribeCmd)
	{
		TObjectPtr<UButtplugFeature> Feature = Device->Features.Add_GetRef(NewObject<UButtplugFeature>(Device));
		Feature->SensorSubscribeCmdIndex = Index;
		Feature->FeatureDescriptor = SensorSubscribeCmd.FeatureDescriptor;
		Feature->SensorRange = SensorSubscribeCmd.SensorRange;
		Feature->FeatureType = Buttplug::Private::GetEnumByName<EButtplugFeatureType>(SensorSubscribeCmd.SensorType);
		++Index;
	}

	Device->SetConnected(true);
	OnDeviceAdded.Broadcast(Device);
}

template<>
void UButtplugSubsystem::OnServerMessage<EButtplugMessageType::DeviceRemoved>(const FButtplugMessage::DeviceRemoved& Message)
{
	if (!Devices.Contains(Message.DeviceIndex))
	{
		UE_LOGFMT(LogButtplug, Warning, "Buttplug server removed device {Index} but we never saw that device added", Message.DeviceIndex);
		return;
	}

	TObjectPtr<UButtplugDevice> Device = Devices[Message.DeviceIndex];
	Device->SetConnected(false);
	OnDeviceRemoved.Broadcast(Device);
}

template<>
void UButtplugSubsystem::OnServerMessage<EButtplugMessageType::SensorReading>(const FButtplugMessage::SensorReading& Message)
{
	if (!Devices.Contains(Message.DeviceIndex))
	{
		UE_LOGFMT(LogButtplug, Warning, "Buttplug server reported sensor reading for device {Index} but we never saw that device added", Message.DeviceIndex);
		return;
	}

	bool bDidBroadcastReading = false;
	TObjectPtr<UButtplugDevice> Device = Devices[Message.DeviceIndex];
	EButtplugFeatureType FeatureType = Buttplug::Private::GetEnumByName<EButtplugFeatureType>(Message.SensorType);

	for (TObjectPtr<UButtplugFeature> Feature : Device->Features)
	{
		if ((Feature->FeatureType == FeatureType) &&
			(Feature->SensorReadCmdIndex == Message.SensorIndex || Feature->SensorSubscribeCmdIndex == Message.SensorIndex))
		{
			if (bDidBroadcastReading)
			{
				UE_LOGFMT(LogButtplug, Warning, "Buttplug protocol encountered ambiguous sensor for device {Index}; see <https://github.com/buttplugio/buttplug/issues/535>", Message.DeviceIndex);
			}
			Feature->SetSensorReading(Message.Data);
			bDidBroadcastReading = true;
		}
	}

	if (!bDidBroadcastReading)
	{
		UE_LOGFMT(LogButtplug, Warning, "Buttplug server reported sensor reading for device {Device}'s {Feature} sensor {Sensor} but we don't know about that sensor", Message.DeviceIndex, Message.SensorType, Message.SensorIndex);
	}
}

void UButtplugSubsystem::OnSocketMessage(const FString& MessageString)
{
	FButtplugMessageArray MessageArray;
	ReadButtplugMessagesFromJson(MessageString, MessageArray);
	for (TUniquePtr<FButtplugMessage>& Message : MessageArray)
	{
		Message->Dispatch([this](auto Message) { OnServerMessage(Message); });
	}
}
