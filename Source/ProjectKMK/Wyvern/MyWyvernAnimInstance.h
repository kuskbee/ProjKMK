// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyDefine.h"
#include "MyWyvernAnimInstance.generated.h"

/**
 * 
 */
class ACharacter;
class UCharacterMovementComponent;

UCLASS()
class PROJECTKMK_API UMyWyvernAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<ACharacter> Character;

	UPROPERTY(VisibleAnywhere, Category = "Components", BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	UPROPERTY(VisibleAnywhere, Category = "State", BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, Category = "State", BlueprintReadOnly)
	uint8 bShouldMove : 1;

	UPROPERTY(VisibleAnywhere, Category = "State", BlueprintReadOnly)
	uint8 bIsFalling : 1;

	UPROPERTY(VisibleAnywhere, Category = "State", BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, Category = "State", BlueprintReadOnly)
	EAIState MonAIState;

	UPROPERTY(VisibleAnywhere, Category = "State", BlueprintReadOnly)
	EPhase Phase;

};
