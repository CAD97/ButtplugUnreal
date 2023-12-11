// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

#include "ButtplugMessage.h"

#include "ButtplugConversions.h"

TUniquePtr<FButtplugMessage> FButtplugMessage::Make(EButtplugMessageType InMessageType)
{
	switch (InMessageType)
	{
		case EButtplugMessageType::Ok:                   return MakeUnique<FButtplugMessage::Ok>();
		case EButtplugMessageType::Error:                return MakeUnique<FButtplugMessage::Error>();
		case EButtplugMessageType::Ping:                 return MakeUnique<FButtplugMessage::Ping>();
		case EButtplugMessageType::RequestServerInfo:    return MakeUnique<FButtplugMessage::RequestServerInfo>();
		case EButtplugMessageType::ServerInfo:           return MakeUnique<FButtplugMessage::ServerInfo>();
		case EButtplugMessageType::StartScanning:        return MakeUnique<FButtplugMessage::StartScanning>();
		case EButtplugMessageType::StopScanning:         return MakeUnique<FButtplugMessage::StopScanning>();
		case EButtplugMessageType::ScanningFinished:     return MakeUnique<FButtplugMessage::ScanningFinished>();
		case EButtplugMessageType::RequestDeviceList:    return MakeUnique<FButtplugMessage::RequestDeviceList>();
		case EButtplugMessageType::DeviceList:           return MakeUnique<FButtplugMessage::DeviceList>();
		case EButtplugMessageType::DeviceAdded:          return MakeUnique<FButtplugMessage::DeviceAdded>();
		case EButtplugMessageType::DeviceRemoved:        return MakeUnique<FButtplugMessage::DeviceRemoved>();
		case EButtplugMessageType::StopDeviceCmd:        return MakeUnique<FButtplugMessage::StopDeviceCmd>();
		case EButtplugMessageType::StopAllDevices:       return MakeUnique<FButtplugMessage::StopAllDevices>();
		case EButtplugMessageType::ScalarCmd:            return MakeUnique<FButtplugMessage::ScalarCmd>();
		case EButtplugMessageType::LinearCmd:            return MakeUnique<FButtplugMessage::LinearCmd>();
		case EButtplugMessageType::RotateCmd:            return MakeUnique<FButtplugMessage::RotateCmd>();
		case EButtplugMessageType::SensorReadCmd:        return MakeUnique<FButtplugMessage::SensorReadCmd>();
		case EButtplugMessageType::SensorReading:        return MakeUnique<FButtplugMessage::SensorReading>();
		case EButtplugMessageType::SensorSubscribeCmd:   return MakeUnique<FButtplugMessage::SensorSubscribeCmd>();
		case EButtplugMessageType::SensorUnsubscribeCmd: return MakeUnique<FButtplugMessage::SensorUnsubscribeCmd>();
		default:                   checkNoEntry();       return nullptr;
	}
}

FString WriteButtplugMessagesToJson(const FButtplugMessageArray& Messages)
{
	FString OutJson;
	WriteButtplugMessagesToJson(Messages, OutJson);
	return OutJson;
}

void WriteButtplugMessagesToJson(const FButtplugMessageArray& Messages, FString& OutJson)
{
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&OutJson);
	JsonWriter->WriteArrayStart();
	for (const TUniquePtr<FButtplugMessage>& Message : Messages)
	{
		FString MessageTypeName = Buttplug::Private::GetEnumAsString(Message->GetMessageType());
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteObjectStart(MessageTypeName);
		Message->ToJson(JsonWriter, /*bFlatObject:*/true);
		JsonWriter->WriteObjectEnd();
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
	verify(JsonWriter->Close());
}


bool ReadButtplugMessagesFromJson(const FString& Json, FButtplugMessageArray& OutMessages)
{
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Json);
	TArray<TSharedPtr<FJsonValue>> JsonValueArray;
	if (!FJsonSerializer::Deserialize(JsonReader, JsonValueArray))
	{
		return false;
	}
	
	OutMessages.Empty();
	bool bSuccess = true;
	for (const TSharedPtr<FJsonValue>& JsonValue : JsonValueArray)
	{
		const TSharedPtr<FJsonObject>& JsonObject = JsonValue->AsObject();
		bSuccess &= JsonObject->Values.Num() != 0;
		for (auto const& Field : JsonObject->Values)
		{
			EButtplugMessageType MessageType;
			if (Buttplug::Private::GetEnumByName(Field.Key, MessageType))
			{
				TUniquePtr<FButtplugMessage>& Message = OutMessages.Add_GetRef(FButtplugMessage::Make(MessageType));
				bSuccess &= Message->FromJson(Field.Value->AsObject());
			}
			else
			{
				bSuccess = false;
				continue;
			}
		}
	}

	return bSuccess;
}
