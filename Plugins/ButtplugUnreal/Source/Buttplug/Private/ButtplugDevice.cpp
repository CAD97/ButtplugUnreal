// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

#include "ButtplugDevice.h"

#include "ButtplugConversions.h"
#include "ButtplugFeature.h"
#include "ButtplugMessage.h"
#include "ButtplugSubsystem.h"

const FString& UButtplugDevice::GetDescriptiveName() const
{
    return DescriptiveName;
}

const FString& UButtplugDevice::GetDisplayName() const
{
    return DisplayName.IsEmpty() ? DescriptiveName : DisplayName;
}

const TArray<TObjectPtr<UButtplugFeature>>& UButtplugDevice::GetFeatures() const
{
    return Features;
}

bool UButtplugDevice::IsConnected() const
{
    return bConnected;
}

void UButtplugDevice::GetActuators(EButtplugFeatureType ActuatorType, TArray<UButtplugFeature*> Actuators) const
{
    Actuators.Empty();
    for (const TObjectPtr<UButtplugFeature>& Feature : Features)
    {
        if (Feature->GetFeatureType() == ActuatorType && Feature->IsActuator())
        {
            Actuators.Add(Feature);
        }
    }
}

void UButtplugDevice::GetSensors(EButtplugFeatureType SensorType, TArray<UButtplugFeature*> Sensors) const
{
    Sensors.Empty();
    for (const TObjectPtr<UButtplugFeature>& Feature : Features)
    {
        if (Feature->GetFeatureType() == SensorType && Feature->IsSensor())
        {
            Sensors.Add(Feature);
        }
    }
}

float UButtplugDevice::GetMessageTimingGap() const
{
    return MessageTimingGapOverride >= 0 ? MessageTimingGapOverride : DefaultMessageTimingGap;
}

void UButtplugDevice::SetMessageTimingGap(float Override)
{
    MessageTimingGapOverride = Override;
}

bool UButtplugDevice::CanVibrate() const
{
    return CanActuate(EButtplugFeatureType::Vibrate);
}

bool UButtplugDevice::CanRotate() const
{
    return CanActuate(EButtplugFeatureType::Rotate);
}

bool UButtplugDevice::CanOscillate() const
{
    return CanActuate(EButtplugFeatureType::Oscillate);
}

bool UButtplugDevice::CanConstrict() const
{
    return CanActuate(EButtplugFeatureType::Constrict);
}

bool UButtplugDevice::CanInflate() const
{
    return CanActuate(EButtplugFeatureType::Inflate);
}

bool UButtplugDevice::CanPosition() const
{
    return CanActuate(EButtplugFeatureType::Position);
}

bool UButtplugDevice::CanActuate(EButtplugFeatureType ActuatorType) const
{
    for (const TObjectPtr<UButtplugFeature>& Feature : Features)
    {
        if (Feature->GetFeatureType() == ActuatorType)
        {
            return true;
        }
    }
    return false;
}

void UButtplugDevice::VibrateAll(double Value, float Duration)
{
    return ActuateAll(EButtplugFeatureType::Vibrate, Value, Duration);
}

void UButtplugDevice::RotateAll(double Value, float Duration)
{
    return ActuateAll(EButtplugFeatureType::Rotate, Value, Duration);
}

void UButtplugDevice::OscillateAll(double Value, float Duration)
{
    return ActuateAll(EButtplugFeatureType::Oscillate, Value, Duration);
}

void UButtplugDevice::ConstrictAll(double Value, float Duration)
{
    return ActuateAll(EButtplugFeatureType::Constrict, Value, Duration);
}

void UButtplugDevice::InflateAll(double Value, float Duration)
{
    return ActuateAll(EButtplugFeatureType::Inflate, Value, Duration);
}

void UButtplugDevice::PositionAll(double Value, float Duration)
{
    return ActuateAll(EButtplugFeatureType::Position, Value, Duration);
}

void UButtplugDevice::ActuateAll(EButtplugFeatureType ActuatorType, double Value, float Duration)
{
    if (!IsConnected()) return;

    for (const TObjectPtr<UButtplugFeature>& Feature : Features)
    {
        if (Feature->GetFeatureType() == ActuatorType && Feature->IsActuator())
        {
            Feature->Actuate(Value, Duration);
        }
    }
}

void UButtplugDevice::StopAll(EButtplugFeatureType ActuatorType)
{
    if (!IsConnected()) return;

    for (const TObjectPtr<UButtplugFeature>& Feature : Features)
    {
        if (Feature->GetFeatureType() == ActuatorType && Feature->IsActuator())
        {
            Feature->Actuate(0.0, INFINITY);
        }
    }
}

void UButtplugDevice::Stop()
{
    if (!IsConnected()) return;

    bHasQueuedStopDevice = true;
}

bool UButtplugDevice::HasBatteryLevel() const
{
    return CanSense(EButtplugFeatureType::Battery);
}

float UButtplugDevice::GetBatteryLevel() const
{
    for (const TObjectPtr<UButtplugFeature>& Feature : Features)
    {
        if (Feature->GetFeatureType() == EButtplugFeatureType::Battery && Feature->IsSensor())
        {
            const TArray<int32>& Reading = Feature->GetLastSensorReading();
            if (Reading.IsEmpty()) break;
            int32 IntLevel = Reading[0];
            FInt32Interval LevelRange = Feature->GetSensorRange()[0];
            return (IntLevel + LevelRange.Min) / float(LevelRange.Size());
        }
    }
    return -1.0f;
}

bool UButtplugDevice::CanSense(EButtplugFeatureType SensorType) const
{
    for (const TObjectPtr<UButtplugFeature>& Feature : Features)
    {
        if (Feature->GetFeatureType() == SensorType && Feature->IsSensor())
        {
            return true;
        }
    }
    return false;
}

UButtplugSubsystem* UButtplugDevice::GetSubsystem() const
{
    return Cast<UButtplugSubsystem>(GetOuter());
}

void UButtplugDevice::SetConnected(bool bInConnected)
{
    bConnected = bInConnected;
    if (bConnected)
    {
        OnConnected.Broadcast();
    }
    else
    {
        OnDisconnected.Broadcast();
    }
}

void UButtplugDevice::FlushMessageQueue(float DeltaTime)
{
    TimeSinceLastMessage += DeltaTime;
    if (TimeSinceLastMessage < GetMessageTimingGap())
    {
        return;
    }

    if (bHasQueuedStopDevice)
    {
        TUniquePtr<FButtplugMessage::StopDeviceCmd> StopCmd = MakeUnique<FButtplugMessage::StopDeviceCmd>();
        StopCmd->DeviceIndex = DeviceIndex;
        GetSubsystem()->EnqueueMessage(MoveTemp(StopCmd));

        for (TObjectPtr<UButtplugFeature> Feature : Features)
        {
            // Cancel any already queued acutation if we're stopping the device this tick.
            Feature->bHasQueuedActuation = false;
        }
    }
    else
    {
        TUniquePtr<FButtplugMessage::LinearCmd> LinearCmd;
        TUniquePtr<FButtplugMessage::RotateCmd> RotateCmd;
        TUniquePtr<FButtplugMessage::ScalarCmd> ScalarCmd;

        for (TObjectPtr<UButtplugFeature> Feature : Features)
        {
            if (Feature->bHasQueuedActuation)
            {
                Feature->bHasQueuedActuation = false;
                checkf(Feature->IsActuator(), TEXT("Should not queue a Buttplug device feature actuation if feature cannot actuate"));

                if (Feature->LinearCmdIndex != INDEX_NONE)
                {
                    if (!LinearCmd)
                    {
                        LinearCmd = MakeUnique<FButtplugMessage::LinearCmd>();
                        LinearCmd->DeviceIndex = DeviceIndex;
                    }
                    FButtplugMessage::Vector Vector;
                    Vector.Index = Feature->LinearCmdIndex;
                    Vector.Duration = FMath::RoundToInt32(Feature->QueuedActuation.Duration * 1000); // convert s -> ms
                    Vector.Position = Feature->QueuedActuation.Value;
                    LinearCmd->Vectors.Add(Vector);
                }
                else if (Feature->RotateCmdIndex != INDEX_NONE)
                {
                    if (!RotateCmd)
                    {
                        RotateCmd = MakeUnique<FButtplugMessage::RotateCmd>();
                        RotateCmd->DeviceIndex = DeviceIndex;
                    }
                    FButtplugMessage::Rotation Rotation;
                    Rotation.Index = Feature->RotateCmdIndex;
                    Rotation.Speed = FMath::Abs(Feature->QueuedActuation.Value);
                    Rotation.Clockwise = Feature->QueuedActuation.Value >= 0;
                    RotateCmd->Rotations.Add(Rotation);
                }
                else if (Feature->ScalarCmdIndex != INDEX_NONE)
                {
                    if (!ScalarCmd)
                    {
                        ScalarCmd = MakeUnique<FButtplugMessage::ScalarCmd>();
                        ScalarCmd->DeviceIndex = DeviceIndex;
                    }
                    FButtplugMessage::Scalar Scalar;
                    Scalar.Index = Feature->ScalarCmdIndex;
                    Scalar.Value = Feature->QueuedActuation.Value;
                    Scalar.ActuatorType = Buttplug::Private::GetEnumAsString(Feature->FeatureType);
                    ScalarCmd->Scalars.Add(Scalar);
                }
            }
        }

        if (LinearCmd) GetSubsystem()->EnqueueMessage(MoveTemp(LinearCmd));
        if (RotateCmd) GetSubsystem()->EnqueueMessage(MoveTemp(RotateCmd));
        if (ScalarCmd) GetSubsystem()->EnqueueMessage(MoveTemp(ScalarCmd));
    }

    for (TUniquePtr<FButtplugMessage>& Cmd : MessageQueue)
    {
        GetSubsystem()->EnqueueMessage(MoveTemp(Cmd));
    }
    MessageQueue.Empty();
}
