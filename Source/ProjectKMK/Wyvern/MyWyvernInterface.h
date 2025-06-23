// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyDefine.h"
#include "MyWyvernInterface.generated.h"
/**
 * 
 */

UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class PROJECTKMK_API UMyWyvernInterface : public UInterface
{
	GENERATED_BODY()
};

class IMyWyvernInterface
{
	GENERATED_BODY()
public:
	virtual bool Attack();
	virtual bool SetAIState(EAIState AIState);
};
