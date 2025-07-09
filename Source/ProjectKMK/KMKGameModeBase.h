// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KMKGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTKMK_API AKMKGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AKMKGameModeBase();


	void NotifyPlayerDead();

protected:
	UPROPERTY()
	int32 TeamDeathCount = 0;
	UPROPERTY(EditDefaultsOnly)
	int32 MaxTeamDeath = 3;
};
