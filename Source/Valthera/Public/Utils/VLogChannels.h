// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

VALTHERA_API DECLARE_LOG_CATEGORY_EXTERN(LogV, Log, All);
VALTHERA_API DECLARE_LOG_CATEGORY_EXTERN(LogVExperience, Log, All);

VALTHERA_API FString GetClientServerContextString(UObject* ContextObject = nullptr);