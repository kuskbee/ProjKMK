// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KMKGameModeBase.generated.h"

class AKMKPlayerController;
class ARespawnPoint;
class APlayerCharacter;

/**
 * 
 */
UCLASS()
class PROJECTKMK_API AKMKGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AKMKGameModeBase();

	UFUNCTION(BlueprintCallable)
	void NotifyPlayerDead(AKMKPlayerController* PlayerControllerThatDied);



protected:
	UPROPERTY()
	int32 TeamDeathCount = 0;
	UPROPERTY(EditDefaultsOnly)
	int32 MaxTeamDeath = 3;
	UPROPERTY(EditDefaultsOnly)
	float RespawnDelay = 8.0f;

	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<ARespawnPoint*> RespawnPoint;

	UPROPERTY()
	TMap<AKMKPlayerController*, FTimerHandle> RespawnTimer;

	ARespawnPoint* GetAvailableRespawnPoint();
	ARespawnPoint* GetAvailableRespawnPoint(AKMKPlayerController* KMKPlayerController);

	void InitiateRespawn(AKMKPlayerController* PlayerControllerToRespawn);
};
