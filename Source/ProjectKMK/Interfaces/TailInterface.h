// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TailInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTailInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTKMK_API ITailInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
	void OnDrop();
};
