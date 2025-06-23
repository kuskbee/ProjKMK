// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacter.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Character = Cast<ACharacter>(TryGetPawnOwner());
	if (Character)
	{
		MovementComponent = Character->GetCharacterMovement();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!IsValid(Character))
		return;

	Velocity = MovementComponent->Velocity;
	GroundSpeed = Velocity.Size2D();

	FVector AccelVector = MovementComponent->GetCurrentAcceleration();
	ShouldMove = GroundSpeed > 3.0f && !AccelVector.Equals(FVector(0.f), 0);
	
	IsFalling = MovementComponent->IsFalling();

	//
	APlayerCharacter* Player = Cast<APlayerCharacter>(Character);
	/*if (Player)
	{
		State = Player->EchoState;
		DeathIndex = Player->DeathIndex;
		IsHold = Player->IsHold;
	}*/
}
