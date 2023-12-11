// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "CoreMinimal.h"
#include "ButtplugMinimal.h"

#include "ButtplugFeature.generated.h"

UENUM(BlueprintType)
enum class EButtplugFeatureType : uint8
{
	Unknown UMETA(Hidden),
	/// Vibration magnitude.
	Vibrate,
	/// Rotation speed.
	Rotate,
	/// Oscillation speed.
	Oscillate,
	/// Constriction pressure.
	Constrict,
	/// Inflation pressure.
	Inflate,
	/// Linear movement.
	Position,
	/// Battery level.
	Battery,
	/// Received Signal Strength Indicator level.
	RSSI,
	/// A button.
	Button,
	/// Linear pressure.
	Pressure,
};

UCLASS(BlueprintType)
class BUTTPLUG_API UButtplugFeature : public UObject
{
	GENERATED_BODY()

private:
	class FLatentSensorAction;
	struct FQueuedActuation
	{
		double Value = 0.0;
		float Duration = 0.0;
	};

	friend class UButtplugDevice;
	friend class UButtplugSubsystem;
	friend class ThisClass::FLatentSensorAction;

public:
	/// Description of the feature.
	UFUNCTION(BlueprintGetter)
	const FString& GetFeatureDescriptor() const { return FeatureDescriptor; }
	/// Type of the feature.
	UFUNCTION(BlueprintGetter)
	EButtplugFeatureType GetFeatureType() const { return FeatureType; }

public:
	/// Is this feature an actuator?
	UFUNCTION(BlueprintCallable)
	bool IsActuator() const;
	/// Is this feature a sensor?
	UFUNCTION(BlueprintCallable)
	bool IsSensor() const;

	/// Is this feature a sensor able to be polled for readings?
	UFUNCTION(BlueprintCallable)
	bool CanRead() const;
	/// Is this feature a sensor able to subscribe to readings?
	UFUNCTION(BlueprintCallable)
	bool CanSubscribe() const;
	
public:
	/// The number of discrete steps this actuator can use.
	UFUNCTION(BlueprintGetter)
	int32 GetActuatorStepCount() const;
	/// The range and dimensionality of values this sensor can return.
	UFUNCTION(BlueprintGetter)
	const TArray<FInt32Interval>& GetSensorRange() const;

public:
	/// Actuate this feature, if possible.
	/// @param Value Actuation target speed/strength/position.
	/// @param Duration How long to actuate for.  If <= 0.f, indefinitely.
	UFUNCTION(BlueprintCallable)
	void Actuate(double Value, float Duration = 0.0);
	/// Stop actuation of this feature.
	UFUNCTION(BlueprintCallable)
	void Stop();
	/// Subscribe to this sensor, if possible.
	UFUNCTION(BlueprintCallable)
	void Subscribe();
	/// Unsubscribe from this sensor.
	UFUNCTION(BlueprintCallable)
	void Unsubscribe();
	/// Get the most recently received reading from this sensor.
	UFUNCTION(BlueprintCallable)
	const TArray<int32>& GetLastSensorReading() const;
	/// Poll a reading from this sensor, if possible.
	UFUNCTION(BlueprintCallable, meta=(Latent, LatentInfo="LatentInfo"))
	void AsyncRead(FLatentActionInfo LatentInfo, TArray<int32>& Reading);
	/// Poll a reading from this sensor, if possible.
	UFUNCTION(BlueprintCallable)
	void Read();

private:
	void EnqueueReadCmd() const;
	void EnqueueSubscribeCmd() const;
	void EnqueueUnsubscribeCmd() const;
	void SetSensorReading(const TArray<int32>& Reading);

public:
	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSensorReading, const TArray<int32>&, Reading);

	/// Called when a new sensor reading is received.
	UPROPERTY(BlueprintAssignable)
	FOnSensorReading OnSensorReading;

public:
	/// The device this is a feature of.
	UFUNCTION(BlueprintCallable)
	UButtplugDevice* GetDevice() const;

private:
	/// Description of the feature.
	UPROPERTY(BlueprintGetter=GetFeatureDescriptor)
	FString FeatureDescriptor;
	/// Type of the feature.
	UPROPERTY(BlueprintGetter=GetFeatureType)
	EButtplugFeatureType FeatureType = EButtplugFeatureType::Unknown;

	/// The number of discrete steps this feature can be actuated at.
	UPROPERTY(BlueprintGetter=GetActuatorStepCount)
	int32 ActuatorStepCount = 0;
	/// The range of values this sensor can return.
	UPROPERTY(BlueprintGetter=GetSensorRange)
	TArray<FInt32Interval> SensorRange;

	TArray<int32> LastSensorReading;
	TArray<FLatentSensorAction*> LatentSensorActions;
	FTimerHandle ResetTimer;

	uint32 ScalarCmdIndex = INDEX_NONE;
	uint32 RotateCmdIndex = INDEX_NONE;
	uint32 LinearCmdIndex = INDEX_NONE;
	uint32 SensorReadCmdIndex = INDEX_NONE;
	uint32 SensorSubscribeCmdIndex = INDEX_NONE;

	bool bHasQueuedActuation = false;
	FQueuedActuation QueuedActuation;
};
