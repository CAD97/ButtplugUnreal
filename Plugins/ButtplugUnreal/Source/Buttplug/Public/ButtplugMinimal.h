// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "CoreMinimal.h"

// Public

enum class EButtplugFeatureType : uint8;

class UButtplugActuator;
class UButtplugAsyncAction;
class UButtplugDevice;
class UButtplugFeature;
class UButtplugSensor;
class UButtplugSubsystem;

BUTTPLUG_API DECLARE_LOG_CATEGORY_EXTERN(LogButtplug, Log, All);

// Private

enum class EButtplugMessageType : uint8;

struct FButtplugMessage;
using FButtplugMessageArray = TArray<TUniquePtr<FButtplugMessage>>;

template<EButtplugMessageType MessageType>
struct TButtplugMessage;
