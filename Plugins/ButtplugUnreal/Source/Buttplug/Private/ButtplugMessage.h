// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "CoreMinimal.h"
#include "ButtplugMinimal.h"

#include "Serialization/JsonSerializerMacros.h"

#include "ButtplugMessage.generated.h"

// Spec Version 3 Patch 3 (2022-12-30)

UENUM()
enum class EButtplugMessageType : uint8
{
	Ok,
	Error,
	Ping,
	RequestServerInfo,
	ServerInfo,
	StartScanning,
	StopScanning,
	ScanningFinished,
	RequestDeviceList,
	DeviceList,
	DeviceAdded,
	DeviceRemoved,
	StopDeviceCmd,
	StopAllDevices,
	ScalarCmd,
	LinearCmd,
	RotateCmd,
	SensorReadCmd,
	SensorReading,
	SensorSubscribeCmd,
	SensorUnsubscribeCmd,
};

/// A message passed between the Buttplug client and server.
struct FButtplugMessage : FJsonSerializable
{
	uint32 Id = 0;

	virtual EButtplugMessageType GetMessageType() const = 0;
	FORCEINLINE static constexpr uint32 SpecVersion() { return 3; }

	static TUniquePtr<FButtplugMessage> Make(EButtplugMessageType MessageType);

	template<typename FunctionType>
	auto Dispatch(const FunctionType& Func);
	template<typename FunctionType>
	auto Dispatch(const FunctionType& Func) const;

	using Ok = TButtplugMessage<EButtplugMessageType::Ok>;
	using Error = TButtplugMessage<EButtplugMessageType::Error>;
	using Ping = TButtplugMessage<EButtplugMessageType::Ping>;
	using RequestServerInfo = TButtplugMessage<EButtplugMessageType::RequestServerInfo>;
	using ServerInfo = TButtplugMessage<EButtplugMessageType::ServerInfo>;
	using StartScanning = TButtplugMessage<EButtplugMessageType::StartScanning>;
	using StopScanning = TButtplugMessage<EButtplugMessageType::StopScanning>;
	using ScanningFinished = TButtplugMessage<EButtplugMessageType::ScanningFinished>;
	using RequestDeviceList = TButtplugMessage<EButtplugMessageType::RequestDeviceList>;
	using DeviceList = TButtplugMessage<EButtplugMessageType::DeviceList>;
	using DeviceAdded = TButtplugMessage<EButtplugMessageType::DeviceAdded>;
	using DeviceRemoved = TButtplugMessage<EButtplugMessageType::DeviceRemoved>;
	using StopDeviceCmd = TButtplugMessage<EButtplugMessageType::StopDeviceCmd>;
	using StopAllDevices = TButtplugMessage<EButtplugMessageType::StopAllDevices>;
	using ScalarCmd = TButtplugMessage<EButtplugMessageType::ScalarCmd>;
	using LinearCmd = TButtplugMessage<EButtplugMessageType::LinearCmd>;
	using RotateCmd = TButtplugMessage<EButtplugMessageType::RotateCmd>;
	using SensorReadCmd = TButtplugMessage<EButtplugMessageType::SensorReadCmd>;
	using SensorReading = TButtplugMessage<EButtplugMessageType::SensorReading>;
	using SensorSubscribeCmd = TButtplugMessage<EButtplugMessageType::SensorSubscribeCmd>;
	using SensorUnsubscribeCmd = TButtplugMessage<EButtplugMessageType::SensorUnsubscribeCmd>;

	enum class ErrorCode : uint8;
	struct Device;
	struct DeviceMessages;
	struct DeviceMessageAttributes;
	struct Scalar;
	struct Vector;
	struct Rotation;
};

template<EButtplugMessageType MessageType>
struct TButtplugMessage : FButtplugMessage
{
	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return MessageType; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }
	static_assert(sizeof(TButtplugMessage) < 0, "Missing specialization for TButtplugMessage");
};

template<typename FunctionType>
auto FButtplugMessage::Dispatch(const FunctionType& Func)
{
	switch (GetMessageType())
	{
		case EButtplugMessageType::Ok:                   return Func(*Cast<FButtplugMessage::Ok>(this));
		case EButtplugMessageType::Error:                return Func(*Cast<FButtplugMessage::Error>(this));
		case EButtplugMessageType::Ping:                 return Func(*Cast<FButtplugMessage::Ping>(this));
		case EButtplugMessageType::RequestServerInfo:    return Func(*Cast<FButtplugMessage::RequestServerInfo>(this));
		case EButtplugMessageType::ServerInfo:           return Func(*Cast<FButtplugMessage::ServerInfo>(this));
		case EButtplugMessageType::StartScanning:        return Func(*Cast<FButtplugMessage::StartScanning>(this));
		case EButtplugMessageType::StopScanning:         return Func(*Cast<FButtplugMessage::StopScanning>(this));
		case EButtplugMessageType::ScanningFinished:     return Func(*Cast<FButtplugMessage::ScanningFinished>(this));
		case EButtplugMessageType::RequestDeviceList:    return Func(*Cast<FButtplugMessage::RequestDeviceList>(this));
		case EButtplugMessageType::DeviceList:           return Func(*Cast<FButtplugMessage::DeviceList>(this));
		case EButtplugMessageType::DeviceAdded:          return Func(*Cast<FButtplugMessage::DeviceAdded>(this));
		case EButtplugMessageType::DeviceRemoved:        return Func(*Cast<FButtplugMessage::DeviceRemoved>(this));
		case EButtplugMessageType::StopDeviceCmd:        return Func(*Cast<FButtplugMessage::StopDeviceCmd>(this));
		case EButtplugMessageType::StopAllDevices:       return Func(*Cast<FButtplugMessage::StopAllDevices>(this));
		case EButtplugMessageType::ScalarCmd:            return Func(*Cast<FButtplugMessage::ScalarCmd>(this));
		case EButtplugMessageType::LinearCmd:            return Func(*Cast<FButtplugMessage::LinearCmd>(this));
		case EButtplugMessageType::RotateCmd:            return Func(*Cast<FButtplugMessage::RotateCmd>(this));
		case EButtplugMessageType::SensorReadCmd:        return Func(*Cast<FButtplugMessage::SensorReadCmd>(this));
		case EButtplugMessageType::SensorReading:        return Func(*Cast<FButtplugMessage::SensorReading>(this));
		case EButtplugMessageType::SensorSubscribeCmd:   return Func(*Cast<FButtplugMessage::SensorSubscribeCmd>(this));
		case EButtplugMessageType::SensorUnsubscribeCmd: return Func(*Cast<FButtplugMessage::SensorUnsubscribeCmd>(this));
		default:                   checkNoEntry();
	}
}

template<typename FunctionType>
auto FButtplugMessage::Dispatch(const FunctionType& Func) const
{
	switch (GetMessageType())
	{
		case EButtplugMessageType::Ok:                   return Func(*Cast<FButtplugMessage::Ok>(this));
		case EButtplugMessageType::Error:                return Func(*Cast<FButtplugMessage::Error>(this));
		case EButtplugMessageType::Ping:                 return Func(*Cast<FButtplugMessage::Ping>(this));
		case EButtplugMessageType::RequestServerInfo:    return Func(*Cast<FButtplugMessage::RequestServerInfo>(this));
		case EButtplugMessageType::ServerInfo:           return Func(*Cast<FButtplugMessage::ServerInfo>(this));
		case EButtplugMessageType::StartScanning:        return Func(*Cast<FButtplugMessage::StartScanning>(this));
		case EButtplugMessageType::StopScanning:         return Func(*Cast<FButtplugMessage::StopScanning>(this));
		case EButtplugMessageType::ScanningFinished:     return Func(*Cast<FButtplugMessage::ScanningFinished>(this));
		case EButtplugMessageType::RequestDeviceList:    return Func(*Cast<FButtplugMessage::RequestDeviceList>(this));
		case EButtplugMessageType::DeviceList:           return Func(*Cast<FButtplugMessage::DeviceList>(this));
		case EButtplugMessageType::DeviceAdded:          return Func(*Cast<FButtplugMessage::DeviceAdded>(this));
		case EButtplugMessageType::DeviceRemoved:        return Func(*Cast<FButtplugMessage::DeviceRemoved>(this));
		case EButtplugMessageType::StopDeviceCmd:        return Func(*Cast<FButtplugMessage::StopDeviceCmd>(this));
		case EButtplugMessageType::StopAllDevices:       return Func(*Cast<FButtplugMessage::StopAllDevices>(this));
		case EButtplugMessageType::ScalarCmd:            return Func(*Cast<FButtplugMessage::ScalarCmd>(this));
		case EButtplugMessageType::LinearCmd:            return Func(*Cast<FButtplugMessage::LinearCmd>(this));
		case EButtplugMessageType::RotateCmd:            return Func(*Cast<FButtplugMessage::RotateCmd>(this));
		case EButtplugMessageType::SensorReadCmd:        return Func(*Cast<FButtplugMessage::SensorReadCmd>(this));
		case EButtplugMessageType::SensorReading:        return Func(*Cast<FButtplugMessage::SensorReading>(this));
		case EButtplugMessageType::SensorSubscribeCmd:   return Func(*Cast<FButtplugMessage::SensorSubscribeCmd>(this));
		case EButtplugMessageType::SensorUnsubscribeCmd: return Func(*Cast<FButtplugMessage::SensorUnsubscribeCmd>(this));
		default:                   checkNoEntry();
	}
}

using FButtplugMessageArray = TArray<TUniquePtr<FButtplugMessage>>;

FString WriteButtplugMessagesToJson(const FButtplugMessageArray& Messages);
void WriteButtplugMessagesToJson(const FButtplugMessageArray& Messages, FString& OutJson);
bool ReadButtplugMessagesFromJson(const FString& Json, FButtplugMessageArray& OutMessages);

template<typename InMessageType UE_REQUIRES(TIsDerivedFrom<InMessageType, FButtplugMessage>::Value)>
FORCEINLINE InMessageType* Cast(FButtplugMessage* Src)
{
	return Src && (Src->GetMessageType() == InMessageType::StaticMessageType())
		? (InMessageType*)Src : nullptr;
}

template<typename InMessageType>
FORCEINLINE InMessageType* ExactCast(FButtplugMessage* Src)
{
	return Cast<InMessageType>(Src);
}

UE_PUSH_MACRO("JSON_SERIALIZE_ENUM_AS_INT");
#define JSON_SERIALIZE_ENUM_AS_INT(JsonName, JsonEnum) \
		if (Serializer.IsLoading()) \
		{ \
			int64 JsonInt = 0; \
			Serializer.Serialize(TEXT(JsonName), JsonInt); \
			JsonEnum = static_cast<decltype(JsonEnum)>(JsonInt); \
		} \
		else \
		{ \
			int64 JsonInt = static_cast<int64>(JsonEnum); \
			Serializer.Serialize(TEXT(JsonName), JsonInt); \
		}

UE_PUSH_MACRO("JSON_SERIALIZE_SERIALIZABLE_FLATTEN");
#define JSON_SERIALIZE_SERIALIZABLE_FLATTEN(JsonName, JsonObject) \
		JsonObject.Serialize(Serializer, true);

UE_PUSH_MACRO("JSON_SERIALIZE_CUSTOM");
#define JSON_SERIALIZE_CUSTOM(JsonName, LoadCustomJson, SaveCustomJson) \
		if (Serializer.IsLoading()) \
		{ \
			if (Serializer.GetObject()->HasField(TEXT(JsonName))) \
			{ \
				TSharedPtr<FJsonValue> JsonValue = Serializer.GetObject()->GetField<EJson::None>(TEXT(JsonName)); \
				this->LoadCustomJson(JsonValue); \
			} \
			else \
			{ \
				this->LoadCustomJson(TSharedPtr<FJsonValue>(nullptr)); \
			} \
		} \
		else \
		{ \
			AsConst(*this).SaveCustomJson(Serializer); \
		}

// Status messages

template<>
struct TButtplugMessage<EButtplugMessageType::Ok> : public FButtplugMessage
{
	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::Ok; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	END_JSON_SERIALIZER;
};

enum class FButtplugMessage::ErrorCode : uint8
{
	Unknown = 0,
	Init = 1,
	Ping = 2,
	Msg = 3,
	Device = 4,
};

template<>
struct TButtplugMessage<EButtplugMessageType::Error> : public FButtplugMessage
{
	FString Message;
	ErrorCode Code = {};

	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::Error; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	JSON_SERIALIZE("ErrorMessage", Message);
	JSON_SERIALIZE_ENUM_AS_INT("ErrorCode", Code);
	END_JSON_SERIALIZER;
};

template<>
struct TButtplugMessage<EButtplugMessageType::Ping> : public FButtplugMessage
{
	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::Ping; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	END_JSON_SERIALIZER;
};

// Handshake messages

template<>
struct TButtplugMessage<EButtplugMessageType::RequestServerInfo> : public FButtplugMessage
{
	FString ClientName;
	uint32 MessageVersion = 0;

	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::RequestServerInfo; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	JSON_SERIALIZE("ClientName", ClientName);
	JSON_SERIALIZE("MessageVersion", MessageVersion);
	END_JSON_SERIALIZER;
};

template<>
struct TButtplugMessage<EButtplugMessageType::ServerInfo> : public FButtplugMessage
{
	FString ServerName;
	uint32 MessageVersion = 0;
	uint32 MaxPingTime = 0;

	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::ServerInfo; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	JSON_SERIALIZE_WITHDEFAULT("ServerName", ServerName, TEXT(""));
	JSON_SERIALIZE("MessageVersion", MessageVersion);
	JSON_SERIALIZE("MaxPingTime", MaxPingTime);
	END_JSON_SERIALIZER;
};

// Enumeration messages

template<>
struct TButtplugMessage<EButtplugMessageType::StartScanning> : public FButtplugMessage
{
	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::StartScanning; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	END_JSON_SERIALIZER;
};

template<>
struct TButtplugMessage<EButtplugMessageType::StopScanning> : public FButtplugMessage
{
	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::StopScanning; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	END_JSON_SERIALIZER;
};

template<>
struct TButtplugMessage<EButtplugMessageType::ScanningFinished> : public FButtplugMessage
{
	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::ScanningFinished; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	END_JSON_SERIALIZER;
};

template<>
struct TButtplugMessage<EButtplugMessageType::RequestDeviceList> : public FButtplugMessage
{
	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::RequestDeviceList; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	END_JSON_SERIALIZER;
};

struct FButtplugMessage::DeviceMessageAttributes : public FJsonSerializable
{
	FString FeatureDescriptor;
	uint32 StepCount = 0;
	FString ActuatorType;
	FString SensorType;
	TArray<FInt32Interval> SensorRange;

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE_WITHDEFAULT("FeatureDescriptor", FeatureDescriptor, TEXT(""));
	JSON_SERIALIZE_WITHDEFAULT("StepCount", StepCount, 0);
	JSON_SERIALIZE_WITHDEFAULT("ActuatorType", ActuatorType, TEXT(""));
	JSON_SERIALIZE_WITHDEFAULT("SensorType", SensorType, TEXT(""));
	JSON_SERIALIZE_CUSTOM("SensorRange", LoadSensorRangeFromJson, SaveSensorRangeToJson);
	END_JSON_SERIALIZER;

private:
	void LoadSensorRangeFromJson(const TSharedPtr<FJsonValue>& JsonValue)
	{
		if (!JsonValue.IsValid())
		{
			SensorRange.Empty();
			return;
		}

		const TArray<TSharedPtr<FJsonValue>>& JsonRanges = JsonValue->AsArray();
		SensorRange.Empty(JsonRanges.Num());
		for (const TSharedPtr<FJsonValue>& JsonRange : JsonRanges)
		{
			const TArray<TSharedPtr<FJsonValue>>& JsonArray = JsonRange->AsArray();
			if (JsonArray.Num() == 2)
			{
				int32 LowerBound = FMath::RoundToInt32(JsonArray[0]->AsNumber());
				int32 UpperBound = FMath::RoundToInt32(JsonArray[1]->AsNumber());
				SensorRange.Add(FInt32Interval(LowerBound, UpperBound));
			}
		}
	}

	void SaveSensorRangeToJson(FJsonSerializerBase& Serializer) const
	{
		unimplemented();
	}
};

struct FButtplugMessage::DeviceMessages : public FJsonSerializable
{
	TArray<DeviceMessageAttributes> ScalarCmd;
	TArray<DeviceMessageAttributes> LinearCmd;
	TArray<DeviceMessageAttributes> RotateCmd;
	TArray<DeviceMessageAttributes> SensorReadCmd;
	TArray<DeviceMessageAttributes> SensorSubscribeCmd;

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE_ARRAY_SERIALIZABLE_WITHDEFAULT("ScalarCmd", ScalarCmd, DeviceMessageAttributes, {});
	JSON_SERIALIZE_ARRAY_SERIALIZABLE_WITHDEFAULT("LinearCmd", LinearCmd, DeviceMessageAttributes, {});
	JSON_SERIALIZE_ARRAY_SERIALIZABLE_WITHDEFAULT("RotateCmd", RotateCmd, DeviceMessageAttributes, {});
	JSON_SERIALIZE_ARRAY_SERIALIZABLE_WITHDEFAULT("SensorReadCmd", SensorReadCmd, DeviceMessageAttributes, {});
	JSON_SERIALIZE_ARRAY_SERIALIZABLE_WITHDEFAULT("SensorSubscribeCmd", SensorSubscribeCmd, DeviceMessageAttributes, {});
	END_JSON_SERIALIZER;
};

struct FButtplugMessage::Device : public FJsonSerializable
{
	FString Name;
	uint32 Index = 0;
	uint32 MessageTimingGap = 0;
	FString DisplayName;
	DeviceMessages Messages;

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("DeviceName", Name);
	JSON_SERIALIZE("DeviceIndex", Index);
	JSON_SERIALIZE_WITHDEFAULT("DeviceMessageTimingGap", MessageTimingGap, 0);
	JSON_SERIALIZE_WITHDEFAULT("DeviceDisplayName", DisplayName, TEXT(""));
	JSON_SERIALIZE_OBJECT_SERIALIZABLE("DeviceMessages", Messages);
	END_JSON_SERIALIZER;
};

template<>
struct TButtplugMessage<EButtplugMessageType::DeviceList> : public FButtplugMessage
{
	TArray<Device> Devices;

	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::DeviceList; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	JSON_SERIALIZE_ARRAY_SERIALIZABLE("Devices", Devices, Device);
	END_JSON_SERIALIZER;
};

template<>
struct TButtplugMessage<EButtplugMessageType::DeviceAdded> : public FButtplugMessage
{
	Device Device;

	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::DeviceAdded; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	JSON_SERIALIZE_SERIALIZABLE_FLATTEN("Device", Device);
	END_JSON_SERIALIZER;
};

template<>
struct TButtplugMessage<EButtplugMessageType::DeviceRemoved> : public FButtplugMessage
{
	uint32 DeviceIndex = 0;

	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::DeviceRemoved; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	JSON_SERIALIZE("DeviceIndex", DeviceIndex);
	END_JSON_SERIALIZER;
};

// Device messages

template<>
struct TButtplugMessage<EButtplugMessageType::StopDeviceCmd> : public FButtplugMessage
{
	uint32 DeviceIndex = 0;

	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::StopDeviceCmd; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	JSON_SERIALIZE("DeviceIndex", DeviceIndex);
	END_JSON_SERIALIZER;
};

template<>
struct TButtplugMessage<EButtplugMessageType::StopAllDevices> : public FButtplugMessage
{
	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::StopAllDevices; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	END_JSON_SERIALIZER;
};

struct FButtplugMessage::Scalar : public FJsonSerializable
{
	uint32 Index = 0;
	double Value = 0.0;
	FString ActuatorType;

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Index", Index);
	JSON_SERIALIZE("Scalar", Value);
	JSON_SERIALIZE("ActuatorType", ActuatorType);
	END_JSON_SERIALIZER;
};

template<>
struct TButtplugMessage<EButtplugMessageType::ScalarCmd> : public FButtplugMessage
{
	uint32 DeviceIndex = 0;
	TArray<Scalar> Scalars;

	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::ScalarCmd; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	JSON_SERIALIZE("DeviceIndex", DeviceIndex);
	JSON_SERIALIZE_ARRAY_SERIALIZABLE("Scalars", Scalars, Scalar);
	END_JSON_SERIALIZER;
};

struct FButtplugMessage::Vector : public FJsonSerializable
{
	uint32 Index = 0;
	uint32 Duration = 0;
	double Position = 0;

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Index", Index);
	JSON_SERIALIZE("Duration", Duration);
	JSON_SERIALIZE("Position", Position);
	END_JSON_SERIALIZER;
};

template<> struct TButtplugMessage<EButtplugMessageType::LinearCmd> : public FButtplugMessage
{
	uint32 DeviceIndex = 0;
	TArray<Vector> Vectors;

	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::LinearCmd; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	JSON_SERIALIZE("DeviceIndex", DeviceIndex);
	JSON_SERIALIZE_ARRAY_SERIALIZABLE("Vectors", Vectors, Vector);
	END_JSON_SERIALIZER;
};

struct FButtplugMessage::Rotation : public FJsonSerializable
{
	uint32 Index = 0;
	double Speed = 0;
	bool Clockwise = false;

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Index", Index);
	JSON_SERIALIZE("Speed", Speed);
	JSON_SERIALIZE("Clockwise", Clockwise);
	END_JSON_SERIALIZER;
};

template<>
struct TButtplugMessage<EButtplugMessageType::RotateCmd> : public FButtplugMessage
{
	uint32 DeviceIndex = 0;
	TArray<Rotation> Rotations;

	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::RotateCmd; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	JSON_SERIALIZE("DeviceIndex", DeviceIndex);
	JSON_SERIALIZE_ARRAY_SERIALIZABLE("Rotations", Rotations, Rotation);
	END_JSON_SERIALIZER;
};

// Sensor messages

template<>
struct TButtplugMessage<EButtplugMessageType::SensorReadCmd> : public FButtplugMessage
{
	uint32 DeviceIndex = 0;
	uint32 SensorIndex = 0;
	FString SensorType;

	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::SensorReadCmd; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	JSON_SERIALIZE("DeviceIndex", DeviceIndex);
	JSON_SERIALIZE("SensorIndex", SensorIndex);
	JSON_SERIALIZE("SensorType", SensorType);
	END_JSON_SERIALIZER;
};

template<>
struct TButtplugMessage<EButtplugMessageType::SensorReading> : public FButtplugMessage
{
	uint32 DeviceIndex = 0;
	uint32 SensorIndex = 0;
	FString SensorType;
	TArray<int32> Data;

	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::SensorReading; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	JSON_SERIALIZE("DeviceIndex", DeviceIndex);
	JSON_SERIALIZE("SensorIndex", SensorIndex);
	JSON_SERIALIZE("SensorType", SensorType);
	JSON_SERIALIZE_ARRAY("Data", Data);
	END_JSON_SERIALIZER;
};

template<>
struct TButtplugMessage<EButtplugMessageType::SensorSubscribeCmd> : public FButtplugMessage
{
	uint32 DeviceIndex = 0;
	uint32 SensorIndex = 0;
	FString SensorType;

	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::SensorSubscribeCmd; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	JSON_SERIALIZE("DeviceIndex", DeviceIndex);
	JSON_SERIALIZE("SensorIndex", SensorIndex);
	JSON_SERIALIZE("SensorType", SensorType);
	END_JSON_SERIALIZER;
};

template<>
struct TButtplugMessage<EButtplugMessageType::SensorUnsubscribeCmd> : public FButtplugMessage
{
	uint32 DeviceIndex = 0;
	uint32 SensorIndex = 0;
	FString SensorType;

	FORCEINLINE static constexpr EButtplugMessageType StaticMessageType() { return EButtplugMessageType::SensorUnsubscribeCmd; }
	virtual EButtplugMessageType GetMessageType() const override { return StaticMessageType(); }

	BEGIN_JSON_SERIALIZER;
	JSON_SERIALIZE("Id", Id);
	JSON_SERIALIZE("DeviceIndex", DeviceIndex);
	JSON_SERIALIZE("SensorIndex", SensorIndex);
	JSON_SERIALIZE("SensorType", SensorType);
	END_JSON_SERIALIZER;
};

// Raw messages (are not provided due to being dangerous)

UE_POP_MACRO("JSON_SERIALIZE_ENUM_AS_INT");
UE_POP_MACRO("JSON_SERIALIZE_SERIALIZABLE_FLATTEN");
UE_POP_MACRO("JSON_SERIALIZE_CUSTOM");
