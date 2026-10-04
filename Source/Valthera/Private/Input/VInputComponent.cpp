// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/VInputComponent.h"

UVInputComponent::UVInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UVInputComponent::AddInputMappings(const UVInputConfig* InputConfig,
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);
}

void UVInputComponent::RemoveInputMappings(const UVInputConfig* InputConfig,
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);
}

void UVInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
