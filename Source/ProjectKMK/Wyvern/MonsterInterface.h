// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MonsterInterface.generated.h"
/**
 * 
 */

class APlayerCharacter;

UINTERFACE(MinimalAPI)
class UMonsterInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTKMK_API IMonsterInterface
{
	GENERATED_BODY()
public:
	virtual float Attack() = 0;
	virtual bool AddTargetActor(AActor* InTarget) = 0;
	virtual void RemoveTargetActor(AActor* InTarget) = 0;
	virtual AActor* ChangeTargetActor() = 0;
	virtual void CheckTargetActors() = 0;
	virtual void CheckTargetActor(AActor* InTarget) = 0;
};
