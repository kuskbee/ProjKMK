// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GroomComponent.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// * Initialize Components
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraBoom->SetupAttachment(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->SetupAttachment(CameraBoom);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));

	// * Initialize Default Values
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()),
		FRotator(0, -90, 0));
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);

	// WeaponTrace
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	// Monster
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* UEIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (UEIC)
	{
		UEIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &APlayerCharacter::OnMove);
		UEIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &APlayerCharacter::OnLook);
		UEIC->BindAction(IA_Jump, ETriggerEvent::Started, this, &APlayerCharacter::OnJump);
		UEIC->BindAction(IA_Jump, ETriggerEvent::Completed, this, &APlayerCharacter::OnStopJump);
		UEIC->BindAction(IA_NormalAttack, ETriggerEvent::Triggered, this, &APlayerCharacter::OnNormalAttack);
	}

}

void APlayerCharacter::OnMove(const FInputActionValue& Value)
{
	FVector2D DirectionVector = Value.Get<FVector2D>();

	FRotator CameraRotation = GetControlRotation();

	FRotator FloorProjectionRotation = FRotator(0, CameraRotation.Yaw, 0);
	FVector CameraForward = UKismetMathLibrary::GetForwardVector(FloorProjectionRotation);
	FloorProjectionRotation = FRotator(0, CameraRotation.Yaw, CameraRotation.Roll);
	FVector CameraRight = UKismetMathLibrary::GetRightVector(FloorProjectionRotation);

	AddMovementInput(CameraForward, DirectionVector.Y);
	AddMovementInput(CameraRight, DirectionVector.X);
}

void APlayerCharacter::OnLook(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

void APlayerCharacter::OnJump(const FInputActionValue& Value)
{
	Jump();
}

void APlayerCharacter::OnStopJump(const FInputActionValue& Value)
{
	StopJumping();
}

void APlayerCharacter::OnNormalAttack(const FInputActionValue& Value)
{
	bool bIsCanAttack = IsCanAttack();
	if (bIsCanAttack)
	{
		ActiveAttack(false);
	}
}

void APlayerCharacter::SetLocomotionState()
{
	EchoState = EPlayerState::EPS_Locomotion;
}

bool APlayerCharacter::IsCanAttack()
{
	return IsCanPlayMontage() && !IsHold;
}

void APlayerCharacter::ActiveAttack(bool bIsDash)
{
	if (bIsDash)
	{
		//:DASH:
	}
	else // Normal
	{
		//:DASH:
		//RemoveSyncPoint

		PlayAttackMontage(false);
	}

	IncreaseAttackIndex();
	EchoState = EPlayerState::EPS_Attack;
}

uint32 APlayerCharacter::IncreaseAttackIndex()
{
	AttackIndex = (AttackIndex + 1) % AttackSkillCount;
	return AttackIndex;
}

bool APlayerCharacter::IsCanPlayMontage()
{
	bool IsPlayingMontage = GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();
	bool IsLocomotion = EchoState == EPlayerState::EPS_Locomotion;
	return !IsPlayingMontage && IsLocomotion;
}

void APlayerCharacter::PlayAttackMontage(bool bIsDash)
{
	UnbindEventAttackMontageEnd();

	UAnimMontage* AttackMontage;
	if (bIsDash)
	{
		AttackMontage = DashAttackMontage;
	}
	else
	{
		AttackMontage = NormalAttackMontage;
	}

	FName Section = FName(*FString::Printf(TEXT("Attack%d"), AttackIndex));

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		AnimInstance->Montage_JumpToSection(Section, AttackMontage);

		AnimInstance->OnMontageEnded.AddDynamic(this, &APlayerCharacter::EventAttackMontageEnd);
	}
}

void APlayerCharacter::UnbindEventAttackMontageEnd()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::EventAttackMontageEnd);
	}
}

void APlayerCharacter::EventAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != NormalAttackMontage && Montage != DashAttackMontage)
	{
		return;
	}

	//:WEAPON:
	//SetCollisionEnable NoCollision

	SetLocomotionState();
	UnbindEventAttackMontageEnd();
}



