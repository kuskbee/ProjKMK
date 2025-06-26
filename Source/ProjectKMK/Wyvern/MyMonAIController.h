// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyMonAIController.generated.h"

/**
 * 
 */

class UAIPerceptionComponent;

UCLASS()
class PROJECTKMK_API AMyMonAIController : public AAIController
{
	GENERATED_BODY()


public:
	AMyMonAIController();

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UAIPerceptionComponent> AIPerception;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
};
