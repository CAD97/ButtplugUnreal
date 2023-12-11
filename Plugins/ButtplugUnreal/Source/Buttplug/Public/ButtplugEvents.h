// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "CoreMinimal.h"
#include "ButtplugMinimal.h"

#include "UObject/Interface.h"

#include "ButtplugEvents.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UButtplugEvents : public UInterface
{
	GENERATED_BODY()
};

/// Automatically subscribed events for the Buttplug Subsystem events when implemented on your GameInstance class.
class BUTTPLUG_API IButtplugEvents
{
	GENERATED_BODY()

public:
	/// Called after connecting to a Bluttplug server.
	UFUNCTION(BlueprintNativeEvent, Category="Buttplug|Server", meta=(DisplayName="Buttplug Server Connected"))
	void OnButtplugServerConnected();
	/// Called after disconnecting from a buttplug server.
	UFUNCTION(BlueprintNativeEvent, Category="Buttplug|Server", meta=(DisplayName="Buttplug Server Disconnected"))
	void OnButtplugServerDisconnected();
	/// Called when the server device scanning times out.
	/// In reality, this event is usually only useful when working with systems that can only scan for a single device at a time.
	/// It should be assumed that StartScanning/StopScanning will be the main usage.
	UFUNCTION(BlueprintNativeEvent, Category="Buttplug|Device", meta=(DisplayName="Buttplug Device Scanning Finished"))
	void OnButtplugDeviceScanningFinished();
	/// Called whenever a device is added to the system.
	UFUNCTION(BlueprintNativeEvent, Category="Buttplug|Device", meta=(DisplayName="Buttplug Device Added"))
	void OnButtplugDeviceAdded(UButtplugDevice* Device);
	/// Called whenever a device is removed from the system.
	UFUNCTION(BlueprintNativeEvent, Category="Buttplug|Device", meta=(DisplayName="Buttplug Device Removed"))
	void OnButtplugDeviceRemoved(UButtplugDevice* Device);
};
