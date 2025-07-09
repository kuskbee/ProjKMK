// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WyvernInterface.generated.h"
/**
 * 
 */

UINTERFACE(MinimalAPI)
class UWyvernInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTKMK_API IWyvernInterface
{
	GENERATED_BODY()
public:
	virtual void Attack() = 0;
};
