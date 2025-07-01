// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWyvernAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WyvernCharacter.h"

void UMyWyvernAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ACharacter>(GetOwningActor());
	if (Character)
	{
		MovementComponent = Character->GetCharacterMovement();
	}
}

void UMyWyvernAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(Character))
	{
		MovementComponent = Character->GetCharacterMovement();

		Velocity = MovementComponent->Velocity;
		GroundSpeed = Velocity.Size2D();

		FVector AccelVector = MovementComponent->GetCurrentAcceleration();

		//bShouldMove = MovementComponent->GetCurrentAcceleration().Size() != 0 && GroundSpeed > 3.0f;
		bShouldMove = GroundSpeed > 3.0f;

		bIsFalling = MovementComponent->IsFalling();

		AWyvernCharacter* WyvernCharacter = Cast<AWyvernCharacter>(Character);
		if (WyvernCharacter)
		{
			MonAIState = WyvernCharacter->MonAIState;
			Phase = WyvernCharacter->Phase;
		}
	}
}
