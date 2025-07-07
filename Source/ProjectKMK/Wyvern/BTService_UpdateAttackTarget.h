// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_UpdateAttackTarget.generated.h"

/**
 * 
 */
class ACharacter;

UCLASS()
class PROJECTKMK_API UBTService_UpdateAttackTarget : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TArray<TObjectPtr<ACharacter>> Players;

};
