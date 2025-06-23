// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GroomComponent.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "MotionWarpingComponent.h"
#include "Weapon/WeaponBase.h"

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
	
	SpawnWeapon();
}

bool APlayerCharacter::ApplyHit(const FHitResult& HitResult, AActor* HitterActor)
{
	if (EchoState == EPlayerState::EPS_Dead)
	{
		return false;
	}

	if (!IsValid(HitterActor))
	{
		return false;
	}

	FVector ImpactPoint = HitResult.ImpactPoint;
	
	// Knockback 될 지 결정 (멀티로 옮기면 서버에서 해야)
	float Chance = FMath::RandRange(0.f, 1.f);
	bool bIsKnockback = false;
	if (Chance <= KnockbackChance)
	{
		UE_LOG(LogTemp, Warning, TEXT("<Knockback HitRact>"));
		bIsKnockback = true;

		FVector KnockbackDirection = ImpactPoint - HitterActor->GetActorLocation();
		KnockbackDirection.Normalize();
		SetKnockbackDirection(KnockbackDirection);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("<Normal HitRact>"));
		RemoveSyncPoint();
	}

	// HitReact Animation 처리
	FVector HitterLocation = HitterActor->GetActorLocation();
	FString SectionName = GetSectionNameFromHitDirection(HitterLocation);
	PlayHitReactMontage(SectionName);
	
	if (AttackCameraShake)
	{
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), AttackCameraShake,
			GetActorLocation(), 0.f, 3400.f);
	}

	// 피격위치로부터 파티클 생성
	SpawnHitReactEffect(ImpactPoint);

	return true;
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
		RemoveSyncPoint();

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

FString APlayerCharacter::GetSectionNameFromHitDirection(FVector HitterLocation)
{
	// 때린 캐릭터로부터 각도 계산
	float ThetaDegree = GetDegreeFromLocation(HitterLocation);

	if (ThetaDegree >= -45.f && ThetaDegree < 45.f)
	{
		return FString(TEXT("Front"));
	}
	if (ThetaDegree >= 45.f && ThetaDegree < 135.f)
	{
		return FString(TEXT("Left"));
	}
	if (ThetaDegree >= 135.f || ThetaDegree <= -135.f)
	{
		return FString(TEXT("Back"));
	}

	return FString(TEXT("Right"));
}

float APlayerCharacter::GetDegreeFromLocation(FVector Location)
{
	FVector ImpactLowered = FVector(Location.X, Location.Y, GetActorLocation().Z);
	FVector ToHit = ImpactLowered - GetActorLocation();
	ToHit.Normalize();

	FVector ForwardVector = GetActorForwardVector();
	bool RightSide = ToHit.Cross(ForwardVector).Z < 0.f;
	float Theta = FMath::Acos(ToHit.Dot(ForwardVector));
	float Degree = FMath::RadiansToDegrees(Theta);

	if (RightSide)
	{
		Degree *= -1.f;
	}

	return Degree;
}

void APlayerCharacter::RemoveSyncPoint()
{
	MotionWarping->RemoveAllWarpTargets();
}

void APlayerCharacter::SetKnockbackDirection(FVector KnockbackDirection)
{
	float RandomKnockbackPower = FMath::RandRange(350.f, 600.f);

	FVector TargetLocation = GetActorLocation() + KnockbackDirection * RandomKnockbackPower;

	MotionWarping->AddOrUpdateWarpTargetFromLocation(FName("EchoHitReactTranslate"), TargetLocation);

	UE_LOG(LogTemp, Warning, TEXT("Knockback power : %f"), RandomKnockbackPower);
}

void APlayerCharacter::SpawnWeapon()
{
	if (WeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;

		FTransform SpawnTransform = GetMesh()->GetSocketTransform(FName(TEXT("weapon")), RTS_World);
		EquippedWeapon = GetWorld()->SpawnActorDeferred<AWeaponBase>(
			WeaponClass,
			SpawnTransform,
			this,
			this,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn	
		);

		if (EquippedWeapon)
		{
			EquippedWeapon->OwnerCharacter = this;
			EquippedWeapon->AttachToComponent(
				GetMesh(),
				FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
				FName(TEXT("weapon"))
			);
			UGameplayStatics::FinishSpawningActor(EquippedWeapon, SpawnTransform);
		}
	}
}

void APlayerCharacter::SpawnHitReactEffect(FVector Location)
{
	if (HitReactEffect) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitReactEffect, Location);
	}
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

void APlayerCharacter::PlayHitReactMontage(FString SectionName)
{
	UnbindEventHitReactMontageEnd();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(FName(SectionName), HitReactMontage);

		AnimInstance->OnMontageEnded.AddDynamic(this, &APlayerCharacter::EventHitReactMontageEnd);
	}
}

void APlayerCharacter::UnbindEventHitReactMontageEnd()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::EventHitReactMontageEnd);
	}
}

void APlayerCharacter::EventAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != NormalAttackMontage && Montage != DashAttackMontage)
	{
		return;
	}

	EquippedWeapon->SetWeaponCollisionEnable(false);
	
	SetLocomotionState();
	UnbindEventAttackMontageEnd();
}

void APlayerCharacter::EventHitReactMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != HitReactMontage)
	{
		return;
	}

	SetLocomotionState();
	UnbindEventHitReactMontageEnd();
}



