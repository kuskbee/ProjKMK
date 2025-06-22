// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCombatReactInterface.generated.h"
/**
 * 
 */

UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class PROJECTKMK_API UMyCombatReactInterface : public UInterface
{
	GENERATED_BODY()
};

class IMyCombatReactInterface
{
	GENERATED_BODY()
public:
	virtual bool ApplyHit(FHitResult HitResult, AActor* HitterActor);
};


