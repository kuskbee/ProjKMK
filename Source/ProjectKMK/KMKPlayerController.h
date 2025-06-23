// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KMKPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTKMK_API AKMKPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AKMKPlayerController();

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	virtual void BeginPlay() override;
};
