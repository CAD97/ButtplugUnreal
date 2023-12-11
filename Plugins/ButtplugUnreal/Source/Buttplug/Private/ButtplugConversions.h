// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "CoreMinimal.h"
#include "ButtplugMinimal.h"

namespace Buttplug::Private
{

template<typename EnumType>
bool GetEnumByName(const FString& InName, EnumType& OutValue, EGetByNameFlags Flags = EGetByNameFlags::None)
{
	static_assert(TIsEnum<EnumType>::Value, "Should only call this with enum types");
	UEnum* Enum = StaticEnum<EnumType>();
	int32 Index = Enum->GetIndexByNameString(InName, Flags);
	if (Index == INDEX_NONE) return false;
	OutValue = static_cast<EnumType>(Enum->GetValueByIndex(Index));
	return true;
}

template<typename EnumType>
EnumType GetEnumByName(const FString& InName, EGetByNameFlags Flags = EGetByNameFlags::None)
{
	static_assert(TIsEnum<EnumType>::Value, "Should only call this with enum types");
	EnumType Value = {};
	GetEnumByName<EnumType>(InName, Value, Flags);
	return Value;
}

template<typename EnumType>
bool GetEnumByName(const FName& InName, EnumType& OutValue, EGetByNameFlags Flags = EGetByNameFlags::None)
{
	static_assert(TIsEnum<EnumType>::Value, "Should only call this with enum types");
	UEnum* Enum = StaticEnum<EnumType>();
	int32 Index = Enum->GetIndexByName(InName, Flags);
	if (Index == INDEX_NONE) return false;
	OutValue = static_cast<EnumType>(Enum->GetValueByIndex(Index));
	return true;
}

template<typename EnumType>
EnumType GetEnumByName(const FName& InName, EGetByNameFlags Flags = EGetByNameFlags::None)
{
	static_assert(TIsEnum<EnumType>::Value, "Should only call this with enum types");
	EnumType Value = {};
	GetEnumByName<EnumType>(InName, Value, Flags);
	return Value;
}

template<typename EnumType>
FString GetEnumAsString(EnumType Value)
{
	static_assert(TIsEnum<EnumType>::Value, "Should only call this with enum types");
	UEnum* Enum = StaticEnum<EnumType>();
	return Enum->GetNameStringByValue(static_cast<int64>(Value));
}

} // namespace Buttplug::Private
