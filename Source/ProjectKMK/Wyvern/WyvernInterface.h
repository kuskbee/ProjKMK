// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Define.h"
#include "WyvernInterface.generated.h"
/**
 * 
 */

UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class PROJECTKMK_API UWyvernInterface : public UInterface
{
	GENERATED_BODY()
};

class IWyvernInterface
{
	GENERATED_BODY()
public:
	virtual bool Attack();
	virtual bool SetAIState(EAIState AIState);
};
