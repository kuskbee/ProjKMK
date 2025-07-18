// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatReactInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatReactInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTKMK_API ICombatReactInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool ApplyHit(const FHitResult& HitResult, AActor* HitterActor);
};
