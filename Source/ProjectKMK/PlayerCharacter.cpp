// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GroomComponent.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "MotionWarpingComponent.h"
#include "Weapon/WeaponBase.h"
#include "StatusComponent.h"
#include "Player/TargetingSystemComponent.h"
#include "Interfaces/TailInterface.h"
#include "Net/UnrealNetwork.h"

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

	StatusComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent"));

	TargetingSystem = CreateDefaultSubobject<UTargetingSystemComponent>(TEXT("TargetSystem"));
	UE_LOG(LogTemp, Warning, TEXT("[Ctor]Creator TargetingSystem = %p"), TargetingSystem.Get());

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

	bReplicates = true;

	SetReplicateMovement(true);
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, EchoState);
	DOREPLIFETIME(APlayerCharacter, IsHold);
	DOREPLIFETIME(APlayerCharacter, AttackIndex);

	DOREPLIFETIME(APlayerCharacter, EquippedWeapon);
	DOREPLIFETIME(APlayerCharacter, AttackTarget);
	DOREPLIFETIME(APlayerCharacter, HoldTail);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		SpawnWeapon();
	}

	BindEventStatusComponent();
	InitializeTargetSystem();
}

bool APlayerCharacter::ApplyHit(const FHitResult& HitResult, AActor* HitterActor)
{
	if (GetLocalRole() != ROLE_Authority) return false;

	if (EchoState == EPlayerState::EPS_Dead) return false;
	if (!IsValid(HitterActor)) return false;

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
		ResetWarpTarget();
	}
	
	EchoState = EPlayerState::EPS_HitReact;

	// HitReact Animation 처리
	FVector HitterLocation = HitterActor->GetActorLocation();
	FString SectionName = GetSectionNameFromHitDirection(HitterLocation);
	
	Multicast_PlayHitReactMontage(SectionName);
	Multicast_SpawnHitReactEffectsAndCameraShake(ImpactPoint);

	return true;
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 타겟모드일 때 타겟 유효 체크
	if (TargetingSystem)
	{
		bool bInvalidTarget = (CurrentTargetingMode == ETargetingMode::ETM_TargetingMode) &&
			false == IsValid(TargetingSystem->LockedOnTarget);
		if (bInvalidTarget)
		{
			ChangeTargetMode(ETargetingMode::ETM_NormalMode);
		}
	}
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
		UEIC->BindAction(IA_NormalAttack, ETriggerEvent::Started, this, &APlayerCharacter::OnNormalAttack);
		UEIC->BindAction(IA_DashAttack, ETriggerEvent::Started, this, &APlayerCharacter::OnDashAttack);
		UEIC->BindAction(IA_LockOn, ETriggerEvent::Started, this, &APlayerCharacter::OnLockOn);
		UEIC->BindAction(IA_Dodge, ETriggerEvent::Started, this, &APlayerCharacter::OnDodge);
		UEIC->BindAction(IA_Hold, ETriggerEvent::Started, this, &APlayerCharacter::OnHold);
	}

}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] Damaged!! Amount : %f"), *this->GetName(), DamageAmount);

	StatusComponent->TakeDamage(DamageAmount);

	return 0.0f;
}

void APlayerCharacter::RespawnCharacter(FVector NewLocation, FRotator NewRotation)
{
	SetActorLocation(NewLocation);
	SetActorRotation(NewRotation);

	if (StatusComponent)
	{
		StatusComponent->InitializeStatus();
	}

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->StopAllMontages(0.2f);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	if (GetCharacterMovement()->MovementMode == MOVE_None)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		EnableInput(PC);
	}

	EchoState = EPlayerState::EPS_Locomotion;
}

void APlayerCharacter::BindEventStatusComponent()
{
	StatusComponent->OnDead.AddDynamic(this, &APlayerCharacter::DoDeath);
}

void APlayerCharacter::DoDeath(bool bIsDeadStatus)
{
	if (GetLocalRole() != ROLE_Authority) return;
	
	EchoState = EPlayerState::EPS_Dead;

	if (!DeathMontage)
	{
		return;
	}

	DeathIndex = FMath::RandRange(0, DeathMontage->GetNumSections() - 1);
	FName SectionName = DeathMontage->GetSectionName(DeathIndex);
	
	Multicast_PlayDeathMontage(SectionName);
}

void APlayerCharacter::OnMove(const FInputActionValue& Value)
{
	if (!IsMovable()) return;

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
	if (!IsMovable()) return;
	Jump();
}

void APlayerCharacter::OnStopJump(const FInputActionValue& Value)
{
	if (!IsMovable()) return;
	StopJumping();
}

void APlayerCharacter::OnNormalAttack(const FInputActionValue& Value)
{
	if (IsCanAttack())
	{
		Server_PerformAttack(false);
	}
}

void APlayerCharacter::OnDashAttack(const FInputActionValue& Value)
{
	if (IsCanAttack())
	{
		Server_PerformAttack(true);
	}
}

void APlayerCharacter::OnLockOn(const FInputActionValue& Value)
{
	switch (CurrentTargetingMode)
	{
	case ETargetingMode::ETM_NormalMode:
		ChangeTargetMode(ETargetingMode::ETM_TargetingMode);
		break;
	case ETargetingMode::ETM_TargetingMode:
		ChangeTargetMode(ETargetingMode::ETM_NormalMode);
		break;
	}
}

void APlayerCharacter::OnDodge(const FInputActionValue& Value)
{
	if (IsCanPlayMontage())
	{
		Server_PerformDodge();
	}
}

void APlayerCharacter::OnHold(const FInputActionValue& Value)
{
	Server_ToggleHold();
}

void APlayerCharacter::Server_PerformAttack_Implementation(bool bIsDash)
{
	if (!IsCanAttack()) return;

	if (bIsDash)
	{
		GetAttackTargetActor();
		if (AttackTarget)
		{
			SetWarpTarget();
		}
		else
		{
			ResetWarpTarget();
		}
	}
	else
	{
		ResetWarpTarget();
	}

	EchoState = EPlayerState::EPS_Attack;
	IncreaseAttackIndex();

	if (EquippedWeapon)
	{
		EquippedWeapon->SetWeaponCollisionEnable(true);
	}

	Multicast_PlayAttackMontage(bIsDash);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &APlayerCharacter::Server_OnAttackMontageEnded);
	}
}

bool APlayerCharacter::Server_PerformAttack_Validate(bool bIsDash)
{
	return IsCanAttack();
}

void APlayerCharacter::Server_PerformDodge_Implementation()
{
	EchoState = EPlayerState::EPS_Dodge;
	Multicast_PlayDodgeMontage();
}

bool APlayerCharacter::Server_PerformDodge_Validate()
{
	return IsCanPlayMontage();
}

void APlayerCharacter::Server_ToggleHold_Implementation()
{
	if (!IsValid(HoldTailClass) || !IsValid(TailClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Warning] HoldTail or SpawnTail Class Unknown"));
		return;
	}

	if (false == IsValid(HoldTail))
	{
		TArray<AActor*> OverlappingTails;
		GetOverlappingActors(OverlappingTails, TailClass);

		if (OverlappingTails.Num() > 0)
		{
			IsHold = true;

			float Distance = 0.f;
			FVector PlayerLoc = GetActorLocation();
			AActor* Tail = UGameplayStatics::FindNearestActor(PlayerLoc, OverlappingTails, Distance);
			if (IsValid(Tail))
			{
				Tail->Destroy();

				FTransform SpawnTransform = GetMesh()->GetSocketTransform(FName(TEXT("holdsocket")), RTS_World);
				HoldTail = GetWorld()->SpawnActorDeferred<AActor>(
					HoldTailClass,
					SpawnTransform,
					this,
					this,
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);

				if (HoldTail)
				{
					UGameplayStatics::FinishSpawningActor(HoldTail, SpawnTransform);
					HoldTail->AttachToComponent(
						GetMesh(),
						FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
						FName(TEXT("holdsocket"))
					);
				}
			}
		}
	}
	else
	{
		IsHold = false;

		FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, true);

		HoldTail->DetachFromActor(DetachmentRules);
		if (HoldTail->GetClass()->ImplementsInterface(UTailInterface::StaticClass()))
		{
			ITailInterface::Execute_OnDrop(HoldTail); // 블루프린트에 정의
		}

		HoldTail = nullptr;
	}
}

bool APlayerCharacter::Server_ToggleHold_Validate()
{
	return true;
}

void APlayerCharacter::OnRep_EchoState()
{
	switch (EchoState)
	{
	case EPlayerState::EPS_Dead:
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EPlayerState::EPS_Attack:
		break;
	case EPlayerState::EPS_Dodge:
		break;
	case EPlayerState::EPS_Locomotion:
		break;
	case EPlayerState::EPS_HitReact:
		break;
	}
}

void APlayerCharacter::OnRep_IsHold()
{
	if (IsValid(EquippedWeapon))
	{
		EquippedWeapon->WeaponMesh->SetVisibility(!IsHold);
	}
	if (IsValid(HoldTail))
	{
		HoldTail->SetActorHiddenInGame(!IsHold);
	}
}

void APlayerCharacter::OnRep_AttackIndex()
{
}

void APlayerCharacter::Multicast_PlayAttackMontage_Implementation(bool bIsDash)
{
	UnbindEventAttackMontageEnd();

	UAnimMontage* AttackMontageToPlay;
	if (bIsDash)
	{
		AttackMontageToPlay = DashAttackMontage;
	}
	else
	{
		AttackMontageToPlay = NormalAttackMontage;
	}

	FName Section = FName(*FString::Printf(TEXT("Attack%d"), AttackIndex));

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontageToPlay)
	{
		AnimInstance->Montage_Play(AttackMontageToPlay);
		AnimInstance->Montage_JumpToSection(Section, AttackMontageToPlay);

		AnimInstance->OnMontageEnded.AddDynamic(this, &APlayerCharacter::EventAttackMontageEnd);
	}
}

void APlayerCharacter::Multicast_PlayHitReactMontage_Implementation(const FString& SectionName)
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

void APlayerCharacter::Multicast_PlayDodgeMontage_Implementation()
{
	UnbindEventDodgeMontageEnd();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DodgeMontage)
	{
		AnimInstance->Montage_Play(DodgeMontage);
		AnimInstance->OnMontageEnded.AddDynamic(this, &APlayerCharacter::EventDodgeMontageEnd);
	}
}

void APlayerCharacter::Multicast_PlayDeathMontage_Implementation(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}
}


void APlayerCharacter::Multicast_SpawnHitReactEffectsAndCameraShake_Implementation(FVector Location)
{
	if (HitReactEffect) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, HitReactEffect, Location);
	}
	if (AttackCameraShake)
	{
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), AttackCameraShake,
			GetActorLocation(), 0.f, 3400.f);
	}
}

void APlayerCharacter::Server_OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != NormalAttackMontage && Montage != DashAttackMontage) return;

	if (EquippedWeapon)
	{
		EquippedWeapon->SetWeaponCollisionEnable(false);
	}

	EchoState = EPlayerState::EPS_Locomotion;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::Server_OnAttackMontageEnded);
	}
}

bool APlayerCharacter::IsMovable()
{
	bool bIsMovable = false;
	switch (EchoState)
	{
	case EPlayerState::EPS_Locomotion:
		bIsMovable = true;
		break;
	case EPlayerState::EPS_Attack:
	case EPlayerState::EPS_Dodge:
	case EPlayerState::EPS_HitReact:
	case EPlayerState::EPS_Dead:
		bIsMovable = false;
		break;
	}

	return bIsMovable;
}

bool APlayerCharacter::IsCanAttack()
{
	return IsCanPlayMontage() && !IsHold;
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

void APlayerCharacter::GetAttackTargetActor()
{
	// Initialize Dash Variables
	TargetActors.Empty();
	TargetScores.Empty();
	AttackOutHits.Empty();
	AttackTarget = nullptr;

	//Sphere Trace를 통해 일정거리 내 Target들 검출
	FVector Location = GetActorLocation();
	FVector Start = Location + FVector(0.f, 0.f, 100.f);
	FVector End = Location + FVector(0.f, 0.f, -100.f);
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2));
	
	TArray<TObjectPtr<AActor>> IgnoreActors;
	IgnoreActors.Add(this);

	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		Start,
		End,
		AttackRange,
		ObjectTypes,
		false,
		IgnoreActors,
		EDrawDebugTrace::ForDuration,
		AttackOutHits,
		true
	);

	if (!bHit)
	{
		return;
	}

	for (FHitResult Hit : AttackOutHits)
	{
		TObjectPtr<AActor> HitActor = Hit.GetActor();

		// Check AttackTarget By Degree
		FVector HitActorLocation = HitActor->GetActorLocation();
		float Degree = GetDegreeFromLocation(HitActorLocation);

		if (FMath::Abs(Degree) <= AttackAngle)
		{
			TargetActors.Add(HitActor);

			float Score = GetTargetScoreByDistance(HitActorLocation);
			Score -= (PrevAttackTarget == HitActor) ? 100.f : 0.f;

			TargetScores.Add(Score);
		}
	}

	// 점수 기반 최종 타겟 선정
	float MaxScore = 0.f;
	int MaxIndex;
	UKismetMathLibrary::MaxOfFloatArray(TargetScores, MaxIndex, MaxScore);

	if (0 <= MaxIndex && MaxIndex < TargetScores.Num())
	{
		AttackTarget = TargetActors[MaxIndex];
		PrevAttackTarget = AttackTarget;

		UE_LOG(LogTemp, Warning, TEXT("Seleted Target : %s"), *AttackTarget->GetName());
	}
}

float APlayerCharacter::GetTargetScoreByDistance(FVector Location)
{
	//공격범위에 따른 대상과의 거리를 0~100까지의 점수로 반환
	float Distance = (float)FVector::Dist2D(Location, GetActorLocation());

	float Score = FMath::Clamp(1.f - (Distance / AttackRange), 0.f, 1.f) * 100.f;

	//UE_LOG(LogTemp, Warning, TEXT("CalcScore, Distance[%f], AttackRange[%f]"), Distance, AttackRange);
	return Score;
}

void APlayerCharacter::SetWarpTarget()
{
	FVector PlayerLoc = GetActorLocation();
	FVector TargetLoc = AttackTarget->GetActorLocation();
	float AcceptanceDistance = 100.f;

	FVector MoveTargetVector = PlayerLoc - TargetLoc;
	MoveTargetVector.Normalize();
	MoveTargetVector = TargetLoc + MoveTargetVector * AcceptanceDistance;

	FVector RotateTargetVector = TargetLoc;
	
	MotionWarping->AddOrUpdateWarpTargetFromLocation(FName(TEXT("TargetMove")), MoveTargetVector);
	MotionWarping->AddOrUpdateWarpTargetFromLocation(FName(TEXT("TargetRotate")), RotateTargetVector);
}

void APlayerCharacter::ResetWarpTarget()
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
			UGameplayStatics::FinishSpawningActor(EquippedWeapon, SpawnTransform);
			EquippedWeapon->OwnerCharacter = this;
			EquippedWeapon->AttachToComponent(
				GetMesh(),
				FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
				FName(TEXT("weapon"))
			);
		}
	}
}

bool APlayerCharacter::IsCanPlayMontage()
{
	bool IsPlayingMontage = GetMesh()->GetAnimInstance()->IsAnyMontagePlaying();
	bool IsLocomotion = EchoState == EPlayerState::EPS_Locomotion;
	return !IsPlayingMontage && IsLocomotion;
}

void APlayerCharacter::UnbindEventAttackMontageEnd()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::EventAttackMontageEnd);
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

void APlayerCharacter::UnbindEventDodgeMontageEnd()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::EventDodgeMontageEnd);
	}
}

void APlayerCharacter::EventAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != NormalAttackMontage && Montage != DashAttackMontage) return;

	if (GetLocalRole() == ROLE_Authority && EquippedWeapon)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->SetWeaponCollisionEnable(false);
		}
		EchoState = EPlayerState::EPS_Locomotion;
	}

	UnbindEventAttackMontageEnd();
}

void APlayerCharacter::EventHitReactMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != HitReactMontage) return;

	if (GetLocalRole() == ROLE_Authority)
	{
		EchoState = EPlayerState::EPS_Locomotion;
	}

	UnbindEventHitReactMontageEnd();
}

void APlayerCharacter::EventDodgeMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != DodgeMontage) return;

	if (GetLocalRole() == ROLE_Authority)
	{
		EchoState = EPlayerState::EPS_Locomotion;
	}

	UnbindEventDodgeMontageEnd();
}

void APlayerCharacter::InitializeTargetSystem()
{
	if (TargetingSystem)
	{
		//TargetingSystem->Initialize(this, FollowCamera);
		TargetingSystem->OnLockOnNewTarget.AddDynamic(this, &APlayerCharacter::SetTargetModeCamera);
		TargetingSystem->OnResetLockedOnTarget.AddDynamic(this, &APlayerCharacter::SetNormalModeCamera);
	}
}

void APlayerCharacter::ChangeTargetMode(ETargetingMode CurrentMode)
{
	CurrentTargetingMode = CurrentMode;
	switch (CurrentTargetingMode)
	{
	case ETargetingMode::ETM_NormalMode:
	{
		TargetingSystem->ResetLockedOnTarget();
	}
	break;
	case ETargetingMode::ETM_TargetingMode:
	{
		TArray<AActor*> TargetEnemies = TargetingSystem->FindAllTargets();
		if (!TargetEnemies.IsEmpty())
		{
			AActor* LockedOnTarget = TargetingSystem->LockOnToTarget();
			// * 여기서도 Target에 Indicator 설정을 하네.
		}
	}
	break;
	}
}

void APlayerCharacter::SetNormalModeCamera()
{
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 0.f);
	CameraBoom->TargetArmLength = 400.f;
}

void APlayerCharacter::SetTargetModeCamera()
{
	// 기존 위치에서 조금 오른쪽 옆으로 빠지고 위로 올라감
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);

	// 화면 가까이
	CameraBoom->TargetArmLength = 150.f;
}
