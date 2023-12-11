// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "CoreMinimal.h"
#include "ButtplugMinimal.h"

#include "Modules/ModuleManager.h"

class FButtplugModule : public IModuleInterface
{
	// IModuleInterface implementation
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
