// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "CoreMinimal.h"
#include "ButtplugMinimal.h"

#include "ButtplugDevice.generated.h"

UCLASS(BlueprintType)
class BUTTPLUG_API UButtplugDevice : public UObject
{
	GENERATED_BODY()

	friend class UButtplugFeature;
	friend class UButtplugSubsystem;

public:
	/// Descriptive name of the device, as taken from the base device configuration file.
	UFUNCTION(BlueprintGetter)
	const FString& GetDescriptiveName() const;
	/// User provided display name for a device. If not provided, same as the descriptive name.
	UFUNCTION(BlueprintGetter)
	const FString& GetDisplayName() const;
	/// Features of this device.
	const TArray<TObjectPtr<UButtplugFeature>>& GetFeatures() const;
	/// Is this device connected and addressable?
	UFUNCTION(BlueprintCallable)
	bool IsConnected() const;

public:
	/// Actuators of this device.
	UFUNCTION(BlueprintCallable)
	void GetActuators(EButtplugFeatureType ActuatorType, TArray<UButtplugFeature*> Actuators) const;
	/// Sensors of this device.
	UFUNCTION(BlueprintCallable)
	void GetSensors(EButtplugFeatureType SensorType, TArray<UButtplugFeature*> Sensors) const;
	/// The delay imposed between sending messages to this device.
	UFUNCTION(BlueprintCallable, meta=(Units="s"))
	float GetMessageTimingGap() const;
	UFUNCTION(BlueprintCallable, meta=(Units="s"))
	/// Manually set the gap between sending messages to this device. A negative value restores the default.
	void SetMessageTimingGap(float Override);

public:
	UDELEGATE()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEvent);

	/// Called when this device is (re)connected to the system.
	UPROPERTY(BlueprintAssignable)
	FOnEvent OnConnected;
	/// Called when this device is disconnected from the system.
	UPROPERTY(BlueprintAssignable)
	FOnEvent OnDisconnected;

public:
	/// Can this device vibrate any features?
	UFUNCTION(BlueprintCallable)
	bool CanVibrate() const;
	/// Can this device rotate any features?
	UFUNCTION(BlueprintCallable)
	bool CanRotate() const;
	/// Can this device oscillate any features?
	UFUNCTION(BlueprintCallable)
	bool CanOscillate() const;
	/// Can this device constrict any features?
	UFUNCTION(BlueprintCallable)
	bool CanConstrict() const;
	/// Can this device inflate any features?
	UFUNCTION(BlueprintCallable)
	bool CanInflate() const;
	/// Can this device position any features?
	UFUNCTION(BlueprintCallable)
	bool CanPosition() const;

	/// Can this device actuate a feature of the given type?
	/// @param ActuatorType The type of feature to actuate.
	UFUNCTION(BlueprintCallable)
	bool CanActuate(EButtplugFeatureType ActuatorType) const;

public:
	/// Vibrate any and all vibration features.
	/// @param Value The vibration speed as a percentage.
	/// @param Duration How long to vibrate for.  If <= 0.f, indefinitely.
	UFUNCTION(BlueprintCallable)
	void VibrateAll(double Value, float Duration = 0.0);
	/// Rotate any and all rotation features.
	/// @param Value The rotation speed as a percentage.  Negative is counterclockwise (subjectively).
	/// @param Duration How long to rotate for.  If <= 0.f, indefinitely.
	UFUNCTION(BlueprintCallable)
	void RotateAll(double Value, float Duration = 0.0);
	/// Oscillate any and all rotation features.
	/// @param Value The oscillation speed as a percentage.
	/// @param Duration How long to oscillate for.  If <= 0.f, indefinitely.
	UFUNCTION(BlueprintCallable)
	void OscillateAll(double Value, float Duration = 0.0);
	/// Constrict any and all constriction features.
	/// @param Value The constriction strength as a percentage.
	/// @param Duration How long to constrict for.  If <= 0.f, indefinitely.
	UFUNCTION(BlueprintCallable)
	void ConstrictAll(double Value, float Duration = 0.0);
	/// Inflate any and all inflation features.
	/// @param Value The inflation strength as a percentage.
	/// @param Duration How long to inflate for.  If <= 0.f, indefinitely.
	UFUNCTION(BlueprintCallable)
	void InflateAll(double Value, float Duration = 0.0);
	/// Reposition any and all positionable features.
	/// @param Value Target position as a percentage.
	/// @param Duration How long the movement should take.
	UFUNCTION(BlueprintCallable)
	void PositionAll(double Value, float Duration);

	/// Actuate any and all features with the specified type.
	/// @param ActuatorType The type of actuator to actuate.
	/// @param Value Actuation target speed/strength/position.
	/// @param Duration How long to actuate for.  If <= 0.f, indefinitely.
	UFUNCTION(BlueprintCallable)
	void ActuateAll(EButtplugFeatureType ActuatorType, double Value, float Duration = 0.0);
	/// Stop all features with the specified type.
	/// @param ActuatorType The type of actuator to stop.
	UFUNCTION(BlueprintCallable)
	void StopAll(EButtplugFeatureType ActuatorType);
	/// Stop all actuation of this device.
	UFUNCTION(BlueprintCallable)
	void Stop();

public:
	/// Does this device report a battery level?
	UFUNCTION(BlueprintCallable)
	bool HasBatteryLevel() const;
	UFUNCTION(BlueprintCallable)
	float GetBatteryLevel() const;

	UFUNCTION(BlueprintCallable)
	bool CanSense(EButtplugFeatureType SensorType) const;

public:
	UButtplugSubsystem* GetSubsystem() const;

private:
	void SetConnected(bool bInConnected = true);
	void FlushMessageQueue(float DeltaTime);

private:
	/// Descriptive name of the device, as taken from the base device configuration file.
	UPROPERTY(BlueprintGetter=GetDescriptiveName)
	FString DescriptiveName;
	/// User provided display name for a device. If not provided, same as the descriptive name.
	UPROPERTY(BlueprintGetter=GetDisplayName)
	FString DisplayName;
	UPROPERTY(meta=(Units="s"))
	float DefaultMessageTimingGap = 0.0;
	UPROPERTY(meta=(Units="s"))
	float MessageTimingGapOverride = -1.0;

	uint32 DeviceIndex = INDEX_NONE;

	bool bConnected = false;
	bool bHasQueuedStopDevice = false;
	FButtplugMessageArray MessageQueue;
	float TimeSinceLastMessage = 0.0f;

	/// Features of this device.
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TArray<TObjectPtr<UButtplugFeature>> Features;
};
