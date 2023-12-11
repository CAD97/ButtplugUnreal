// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

#include "ButtplugFeature.h"

#include "ButtplugConversions.h"
#include "ButtplugDevice.h"
#include "ButtplugMessage.h"
#include "ButtplugSubsystem.h"
#include "LatentActions.h"

class UButtplugFeature::FLatentSensorAction : public FPendingLatentAction
{
private:
	friend class UButtplugFeature;

public:
	FLatentSensorAction(const FLatentActionInfo& LatentInfo, TObjectPtr<UButtplugFeature> Feature, TArray<int32>& OutReading)
		: FPendingLatentAction()
		, ExecutionFunction(LatentInfo.ExecutionFunction)
		, LinkID(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
		, OwningFeature(Feature)
		, Reading(OutReading)
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
		OwningFeature->LatentSensorActions.Remove(this);
	}

	virtual void NotifyActionAborted()
	{
		OwningFeature->LatentSensorActions.Remove(this);
	}

#if WITH_EDITOR
	virtual FString GetDescription() const override
	{
		return TEXT("Reading Buttplug device sensor");
	}
#endif

private:
	bool bReady = false;
	FName ExecutionFunction;
	int32 LinkID = INDEX_NONE;
	FWeakObjectPtr CallbackTarget;
	TObjectPtr<UButtplugFeature> OwningFeature;
	TArray<int32>& Reading;
};

bool UButtplugFeature::IsActuator() const
{
	return ScalarCmdIndex != INDEX_NONE || RotateCmdIndex != INDEX_NONE || LinearCmdIndex != INDEX_NONE;
}

bool UButtplugFeature::IsSensor() const
{
	return SensorReadCmdIndex != INDEX_NONE || SensorSubscribeCmdIndex != INDEX_NONE;
}

bool UButtplugFeature::CanRead() const
{
	return SensorReadCmdIndex != INDEX_NONE;
}

bool UButtplugFeature::CanSubscribe() const
{
	return SensorSubscribeCmdIndex != INDEX_NONE;
}

int32 UButtplugFeature::GetActuatorStepCount() const
{
	return ActuatorStepCount;
}

const TArray<FInt32Interval>& UButtplugFeature::GetSensorRange() const
{
	return SensorRange;
}

void UButtplugFeature::Actuate(double Value, float Duration)
{
	if (IsActuator())
	{
		QueuedActuation.Duration = Duration;
		QueuedActuation.Value = Value;
		bHasQueuedActuation = true;
		GetWorld()->GetTimerManager().SetTimer(ResetTimer, this, &ThisClass::Stop, Duration);
	}
}

void UButtplugFeature::Stop()
{
	Actuate(0);
}

void UButtplugFeature::Subscribe()
{
	if (CanSubscribe())
	{
		EnqueueSubscribeCmd();
	}
}

void UButtplugFeature::Unsubscribe()
{
	if (CanSubscribe())
	{
		EnqueueUnsubscribeCmd();
	}
}

const TArray<int32>& UButtplugFeature::GetLastSensorReading() const
{
	return LastSensorReading;
}

void UButtplugFeature::AsyncRead(FLatentActionInfo LatentInfo, TArray<int32>& Reading)
{
	FLatentActionManager& LatentActionManager = GetWorld()->GetLatentActionManager();
	if (!LatentActionManager.FindExistingAction<FLatentSensorAction>(LatentInfo.CallbackTarget, LatentInfo.UUID))
	{
		FLatentSensorAction* Action = new FLatentSensorAction(LatentInfo, this, Reading);
		LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
		LatentSensorActions.Add(Action);
		
		if (LatentSensorActions.Num() == 1)
		{
			Read();
		}
	}
}

void UButtplugFeature::Read()
{
	if (CanRead())
	{
		EnqueueReadCmd();
	}
	else
	{
		SetSensorReading({});
	}
}

void UButtplugFeature::EnqueueReadCmd() const
{
	UButtplugDevice* Device = GetDevice();
	TUniquePtr<FButtplugMessage::SensorReadCmd> ReadCmd = MakeUnique<FButtplugMessage::SensorReadCmd>();
	ReadCmd->DeviceIndex = Device->DeviceIndex;
	ReadCmd->SensorIndex = SensorReadCmdIndex;
	ReadCmd->SensorType = Buttplug::Private::GetEnumAsString(FeatureType);
	Device->MessageQueue.Add(MoveTemp(ReadCmd));
}

void UButtplugFeature::EnqueueSubscribeCmd() const
{
	UButtplugDevice* Device = GetDevice();
	TUniquePtr<FButtplugMessage::SensorSubscribeCmd> SubscribeCmd = MakeUnique<FButtplugMessage::SensorSubscribeCmd>();
	SubscribeCmd->DeviceIndex = Device->DeviceIndex;
	SubscribeCmd->SensorIndex = SensorReadCmdIndex;
	SubscribeCmd->SensorType = Buttplug::Private::GetEnumAsString(FeatureType);
	Device->MessageQueue.Add(MoveTemp(SubscribeCmd));
}

void UButtplugFeature::EnqueueUnsubscribeCmd() const
{
	UButtplugDevice* Device = GetDevice();
	TUniquePtr<FButtplugMessage::SensorUnsubscribeCmd> UnsubscribeCmd = MakeUnique<FButtplugMessage::SensorUnsubscribeCmd>();
	UnsubscribeCmd->DeviceIndex = Device->DeviceIndex;
	UnsubscribeCmd->SensorIndex = SensorReadCmdIndex;
	UnsubscribeCmd->SensorType = Buttplug::Private::GetEnumAsString(FeatureType);
	Device->MessageQueue.Add(MoveTemp(UnsubscribeCmd));
}

void UButtplugFeature::SetSensorReading(const TArray<int32>& Reading)
{
	LastSensorReading = Reading;
	OnSensorReading.Broadcast(LastSensorReading);
	for (FLatentSensorAction* Action : LatentSensorActions)
	{
		if (Action)
		{
			Action->Reading = LastSensorReading;
			Action->bReady = true;
		}
	}
	LatentSensorActions.Empty();
}

UButtplugDevice* UButtplugFeature::GetDevice() const
{
	return Cast<UButtplugDevice>(GetOuter());
}
