// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeleeCore.h"

#define LOCTEXT_NAMESPACE "FMeleeCoreModule"

void FMeleeCoreModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("Melee core loaded!"));
	// This code will execute after your module is loaded into Remember; the exact timing is specified in the .uplugin file per-module
}

void FMeleeCoreModule::ShutdownModule()
{
	UE_LOG(LogTemp, Warning, TEXT("Melee core Shut Down!"));
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMeleeCoreModule, MeleeCore)