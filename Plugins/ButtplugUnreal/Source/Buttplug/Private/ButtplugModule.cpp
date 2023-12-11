// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

#include "ButtplugModule.h"

void FButtplugModule::StartupModule()
{
	// This code will execute after your module is loaded into memory.
}

void FButtplugModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.
}
	
IMPLEMENT_MODULE(FButtplugModule, Buttplug)
DEFINE_LOG_CATEGORY(LogButtplug)
