// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "CoreMinimal.h"
#include "ButtplugMinimal.h"

#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"

#include "ButtplugSubsystem.generated.h"

UENUM()
enum class EButtplugClientStartResult : uint8
{
	/// Connection succeeded.
	Succeeded,
	/// Connection failed.
	ConnectionFailed,
};

UCLASS()
class BUTTPLUG_API UButtplugSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

private:
	class FLatentStartAction;
	friend class ThisClass::FLatentStartAction;
	
	// USubsystem implementation
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// State accessors
public:
	/// Is the game connected to a Buttplug server?
	UFUNCTION(BlueprintCallable)
	bool IsConnected() const;

	/// The client name used to connect to the Buttplug server.
	UFUNCTION(BlueprintGetter)
	const FString& GetClientName() const;
	/// The name of the connected Buttplug server.
	UFUNCTION(BlueprintGetter)
	const FString& GetServerName() const;
	/// The address of the connected Buttplug server.
	UFUNCTION(BlueprintGetter)
	const FString& GetServerAddress() const;

	/// The devices connected to this Buttplug server.
	UFUNCTION(BlueprintCallable)
	void GetDevices(TArray<UButtplugDevice*>& Devices) const;
	/// All devices known to this client, including disconnected ones.
	UFUNCTION(BlueprintCallable)
	void GetAllDevices(TArray<UButtplugDevice*>& Devices) const;

	// Events
public:
	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEvent);
	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeviceEvent, UButtplugDevice*, Device);

	/// Called after connecting to a Bluttplug server.
	UPROPERTY(BlueprintAssignable)
	FOnEvent OnConnected;
	/// Called after disconnecting from a Buttplug server.
	UPROPERTY(BlueprintAssignable)
	FOnEvent OnDisconnected;
	/// Called when the server device scanning times out.
	/// In reality, this event is usually only useful when working with systems that can only scan for a single device at a time.
	/// It should be assumed that StartScanning/StopScanning will be the main usage.
	UPROPERTY(BlueprintAssignable)
	FOnEvent OnScanningFinished;
	/// Called whenever a device is added to the system.
	UPROPERTY(BlueprintAssignable)
	FOnDeviceEvent OnDeviceAdded;
	/// Called whenever a device is removed from the system.
	UPROPERTY(BlueprintAssignable)
	FOnDeviceEvent OnDeviceRemoved;

	// Messages
public:
	/// Client request to have the server start scanning for devices on all busses that it knows about.
	/// Useful for protocols like Bluetooth, which require an explicit discovery phase.
	UFUNCTION(BlueprintCallable)
	void StartScanning();
	/// Client request to have the server stop scanning for devices.
	/// Useful for protocols like Bluetooth, which may not timeout otherwise.
	UFUNCTION(BlueprintCallable)
	void StopScanning();

	// Connection
public:
	/// Start the Buttplug client, connecting to a Buttplug server via websocket.
	/// @param ClientName The client name shown to the server (can include the tokens {GameName}, {AppName} or {BuildConfiguration}, which will be replaced)
	/// @param ServerAddress The websocket address to connect on.
	/// @param ErrorMessage The error message if connection fails.
	UFUNCTION(BlueprintCallable,
		meta=(Latent, LatentInfo="LatentInfo", AdvancedDisplay=4, ExpandEnumAsExecs="Result"))
	void AsyncStartClient(FLatentActionInfo LatentInfo, EButtplugClientStartResult& Result, FString& ErrorMessage, const FString& ClientName = TEXT("{AppName} - {GameName} ({BuildConfiguration})"), const FString& ServerAddress = TEXT("ws://127.0.0.1:12345"));
	/// Start the Buttplug client, connecting to a Buttplug server via websocket.
	/// @param ClientName The client name shown to the server.
	/// @param ServerAddress The websocket address to connect on.
	UFUNCTION(BlueprintCallable)
	void StartClient(const FString& ClientName, const FString& ServerAddress = TEXT("ws://127.0.0.1:12345"));
	/// Stop the Buttplug client, disconnecting from the Buttplug server.
	UFUNCTION()
	void StopClient();

	// FTickableGameObject implementation
public:
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;
	virtual bool IsAllowedToTick() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;

	// Lifecycle helpers
public:
	void EnqueueMessage(TUniquePtr<FButtplugMessage> Message);
private:
	void StartPingTimer(float PingRate);
	void TickPingTimer();
	void Reset(const FString& Reason);

	// Socket callbacks
private:
	void OnSocketConnected();
	void OnSocketConnectionError(const FString& Error);
	void OnSocketClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
	template<EButtplugMessageType MessageType>
	void OnServerMessage(const TButtplugMessage<MessageType>& Message);
	void OnSocketMessage(const FString& MessageString);

private:
	bool bInitialized = false;

	/// The client name used to connect to the Buttplug server.
	UPROPERTY(BlueprintGetter=GetClientName)
	FString ClientName;
	/// The name of the connected Buttplug server.
	UPROPERTY(BlueprintGetter=GetServerName)
	FString ServerName;
	/// The address of the connected Buttplug server.
	UPROPERTY(BlueprintGetter=GetServerAddress)
	FString ServerAddress;

	FTimerHandle PingTimer;
	uint32 NextMessageId = 1;
	FButtplugMessageArray MessageBuffer;
	TSharedPtr<class IWebSocket> WebSocket;
	FLatentStartAction* LatentStartAction = nullptr;

	UPROPERTY()
	TMap<uint32, TObjectPtr<UButtplugDevice>> Devices;
};
