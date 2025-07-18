// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "PlayerAnimInstance.generated.h"

class ACharacter;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class PROJECTKMK_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ACharacter> Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool ShouldMove;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPlayerState State = EPlayerState::EPS_Locomotion;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int DeathIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsHold;
};
