// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWyvernAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WyvernCharacter.h"

void UMyWyvernAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ACharacter>(TryGetPawnOwner());
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
		Velocity = MovementComponent->Velocity;
		GroundSpeed = Velocity.Size2D();

		if (MovementComponent->GetCurrentAcceleration().Size() > 0 && GroundSpeed > 3.0f)
		{
			bShouldMove = true;
		}

		bIsFalling = MovementComponent->IsFalling();

		AWyvernCharacter* WyvernCharacter = Cast<AWyvernCharacter>(Character);
		if (IsValid(WyvernCharacter))
		{
			MonAIState = WyvernCharacter->MonAIState;
			Phase = WyvernCharacter->Phase;
		}
	}
}
