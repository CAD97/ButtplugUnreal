// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "CoreMinimal.h"
#include "ButtplugMinimal.h"

#include "LatentActions.h"

class FLatentButtplugSensorReading : public FPendingLatentAction
{
private:
	friend class UButtplugFeature;

public:
	FLatentSensorAction(const FLatentActionInfo& LatentInfo, TObjectPtr<UButtplugFeature> Feature)
		: FPendingLatentAction()
		, ExecutionFunction(LatentInfo.ExecutionFunction)
		, LinkID(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
		, OwningFeature(Feature)
	{
	}

protected:
	virtual void SetReading(const TArray<int32>& Reading)
	{
		bReady = true;
	}

	// FPendingLatentAction implementation
public:
	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		Response.FinishAndTriggerIf(bReady, ExecutionFunction, LinkID, CallbackTarget);
	}

	virtual void NotifyObjectDestroyed()
	{
		OwningFeature->LatentSensorReadings.Remove(this);
	}

	virtual void NotifyActionAborted()
	{
		OwningFeature->LatentSensorReadings.Remove(this);
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
};
