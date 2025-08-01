﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "WyvernCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionWarpingComponent.h"
#include "MonStateComponent.h"
#include "ST_MyMonsterSkill.h"
#include "Animation/AnimMontage.h"
#include "MySurface.h"
#include "Kismet/GameplayStatics.h"
#include "MyLegacyCameraShake.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Particles/ParticleSystem.h"
#include "MyMonAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../PlayerCharacter.h"
#include "../UI/MyHUD.h"
#include "../InGameGameState.h"


// Sets default values
AWyvernCharacter::AWyvernCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetWorldScale3D(FVector(0.3f, 0.3f, 0.3f));
	GetMesh()->SetWorldLocation(FVector(0.0f, 0.0f, -500.0f));

	GetCapsuleComponent()->SetCapsuleSize(500.0f, 500.0f);
	GetCapsuleComponent()->SetWorldScale3D(FVector(0.7f, 0.7f, 0.7f));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetCapsuleComponent());

	SpringArm->AddRelativeLocation(FVector(-3.158533, 25.579432, -52.963234));
	SpringArm->AddRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 1200.0f;

	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("DeadCamera"));
	Camera->SetupAttachment(SpringArm);

	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));

	MonStateComponent = CreateDefaultSubobject<UMonStateComponent>(TEXT("MonStateComponent"));

	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.0f, 0.f);
}

// Called when the game starts or when spawned
void AWyvernCharacter::BeginPlay()
{
	Super::BeginPlay();

	MonStateComponent->SetMonState(Phase);

	MonStateComponent->EventDispatcher_Death.AddDynamic(this,
		&AWyvernCharacter::DoDeath);

	OnTakePointDamage.AddDynamic(this,
		&AWyvernCharacter::EventProcessTakePointDamage);

	if (Phase == EPhase::ThirdPhase)
	{
		CutTail(false);
	}
}

// Called every frame
void AWyvernCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (MonAIState == EAIState::Runaway)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying, 0);
		AddActorWorldOffset(FVector(0, 0, DeltaTime * 300.0f));

		if (GetActorLocation().Z > 3000.0f)
		{
			if (IsValid(TailSurface))
			{
				TailSurface->Destroy();
			}
			Destroy();
		}

	}
}

// Called to bind functionality to input
void AWyvernCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AWyvernCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

}

float AWyvernCharacter::Attack()
{
	TArray<FName> Names;
	FST_MyMonsterSkill* Skill;
	UAnimMontage* AttackMontage = nullptr;
	int RandomIndex = 0;
	float PlayerRate = 0.0f;

	switch (Phase)
	{
	case EPhase::FirstPhase:
		if (FirstPhaseTable)
		{
			Names = FirstPhaseTable->GetRowNames();
			RandomIndex = FMath::RandRange(0, Names.Num() - 1);
			Skill = FirstPhaseTable->FindRow<FST_MyMonsterSkill>(Names[RandomIndex], Names[RandomIndex].ToString());
			AttackMontage = Skill->SkillAnimMontage;
			Damage = Skill->Damage;
			PlayerRate = 1.2f;
		}
		break;

	case EPhase::SecondPhase:
		if (SecondPhaseTable)
		{
			Names = SecondPhaseTable->GetRowNames();
			RandomIndex = FMath::RandRange(0, Names.Num() - 1);
			Skill = SecondPhaseTable->FindRow<FST_MyMonsterSkill>(Names[RandomIndex], Names[RandomIndex].ToString());
			AttackMontage = Skill->SkillAnimMontage;
			Damage = Skill->Damage;
			PlayerRate = 1.2f;
		}
		break;
	case EPhase::ThirdPhase:
		if (ThirdPhaseTable)
		{
			Names = ThirdPhaseTable->GetRowNames();
			RandomIndex = FMath::RandRange(0, Names.Num() - 1);
			Skill = ThirdPhaseTable->FindRow<FST_MyMonsterSkill>(Names[RandomIndex], Names[RandomIndex].ToString());
			AttackMontage = Skill->SkillAnimMontage;
			Damage = Skill->Damage;
			PlayerRate = 1.4f;
		}
		break;
	}

	if (AttackMontage)
	{
		S2A_OnAttack(AttackMontage, 1.4f);
		return AttackMontage->GetPlayLength();
	}
	
	return 0.0f;
}

void AWyvernCharacter::S2A_OnAttack_Implementation(UAnimMontage* InAttackMontage, float InPlayerRate)
{
	if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		if (InAttackMontage)
		{
			PlayAnimMontage(InAttackMontage, InPlayerRate);
		}
	}
}

bool AWyvernCharacter::AddTargetActor(AActor* InTarget)
{
	if (TargetActors.Contains(InTarget))
	{
		return false;
	}

	TargetActors.Add(InTarget);

	APlayerCharacter* PlayCharacter = Cast<APlayerCharacter>(InTarget);
	if (PlayCharacter)
	{
		if (Phase == EPhase::ThirdPhase)
		{
			ShowMonsterHealthBar(PlayCharacter);
		}
	}
	return true;
}

void AWyvernCharacter::RemoveTargetActor(AActor* InTarget)
{
	if (TargetActors.Contains(InTarget))
	{
		TargetActors.Remove(InTarget);

		APlayerCharacter* PlayCharacter = Cast<APlayerCharacter>(InTarget);
		if (PlayCharacter)
		{
			HideMonsterHealthBar(PlayCharacter);
		}
	}
}

AActor* AWyvernCharacter::ChangeTargetActor()
{
	if (TargetActors.Num() <= 0)
	{
		return nullptr;
	}
	AActor* NewTarget = TargetActors[(int)FMath::RandRange(0, TargetActors.Num() - 1)];
	return NewTarget;
}

void AWyvernCharacter::CheckTargetActors()
{

	for (AActor* Target : TargetActors)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(Target);
		if (!Player)
		{
			RemoveTargetActor(Player);
			return CheckTargetActors();
		}
	}
}

void AWyvernCharacter::CheckTargetActor(AActor* InTarget)
{
	if (TargetActors.Contains(InTarget))
	{
		TargetActors.Remove(InTarget);
	}
}

void AWyvernCharacter::DoAttack(bool IsRightHand, bool IsMouth)
{
	FVector AttackLocation;
	if (IsRightHand)
	{
		AttackLocation = GetMesh()->GetSocketLocation("Bip001_R_Hand");
	}
	else
	{
		if (IsMouth)
		{
			AttackLocation = GetMesh()->GetSocketLocation("mouth_socket");
		}
		else
		{
			AttackLocation = GetMesh()->GetSocketLocation("horn_socket");
		}
	}

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));

	TArray<AActor*> ActorToIgnore;
	ActorToIgnore.Add(this);

	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		AttackLocation,
		AttackLocation,
		250.0f,
		ObjectTypes,
		false,
		ActorToIgnore,
		EDrawDebugTrace::None,
		OutHits,
		true
	);

	TMap<AActor*, FHitResult> HitResults;

	for (const FHitResult& Hit : OutHits)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && !HitResults.Contains(HitActor))
		{
			HitResults.Add(HitActor, Hit);
		}
	}

	for (auto& HitResult : HitResults)
	{
		AActor* Actor = HitResult.Key;
		const FHitResult& Hit = HitResult.Value;

		UGameplayStatics::ApplyDamage(Actor, Damage, GetController(), this, nullptr);

		ICombatReactInterface* Object = Cast<ICombatReactInterface>(Actor);
		if (Object)
		{
			Object->ApplyHit(Hit, this);
		}
	}
}

void AWyvernCharacter::EventProcessTakePointDamage(AActor* DamagedActor, float In_Damage,
	AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
	const UDamageType* DamageType, AActor* DamageCauser)
{
	if (MonAIState == EAIState::Patrol)
	{
		AMyMonAIController* MonController = Cast<AMyMonAIController>(GetController());
		APlayerCharacter* InstigatePlayer = Cast<APlayerCharacter>(InstigatedBy->GetPawn());
		if (MonController && InstigatePlayer)
		{
			MonController->AddTargetActor(InstigatePlayer);
		}
	}

	MonStateComponent->AddDamage(In_Damage, BoneName, Phase);
}

bool AWyvernCharacter::ApplyHit(const FHitResult& HitResult, AActor* HitterActor)
{
	if (!MonStateComponent->IsDeath())
	{
		float WarpingPercent =  FMath::FRandRange(0.0f, 1.0f);
		float RandomKnockbackPower = FMath::FRandRange(300.0f, 600.0f);
		S2A_ApplyHit(HitResult, HitterActor, WarpingPercent, RandomKnockbackPower);
	}
	return true;
}

void AWyvernCharacter::S2A_ApplyHit_Implementation(const FHitResult& HitResult, AActor* HitterActor, float WarpingPercent, float RandomKnockbackPower)
{
	AMyMonAIController* MyController = Cast<AMyMonAIController>(GetController());
	if (MyController)
	{
		MyController->WhenPawnTakeDamage(KnockBackMontage->GetPlayLength());
	}
	
	if (KnockBackMontage)
	{
		PlayAnimMontage(KnockBackMontage);
	}

	if (WarpingPercent <= 0.7f)
	{
		MotionWarping->AddOrUpdateWarpTargetFromLocation(
			FName("MonsterKnockbackMove"),
			GetActorLocation() + ((GetActorLocation() - HitterActor->GetActorLocation()).Normalize(0.0001)
				* RandomKnockbackPower));
	}
	else
	{
		MotionWarping->RemoveAllWarpTargets();
	}

	UGameplayStatics::PlayWorldCameraShake(
		GetWorld(),
		UMyLegacyCameraShake::StaticClass(),
		GetActorLocation(),
		0.0f,
		3500.0f,
		1.0f
	);

	if (IsWeakAttack(HitResult.BoneName))
	{
		if (WeakAttackEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), WeakAttackEffect, HitResult.ImpactPoint);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("WeakAttackEffect is null"));
		}
	}
	else
	{
		if (NotWeakAttackEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), NotWeakAttackEffect, HitResult.ImpactPoint);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NotWeakAttackEffect is null"));
		}
	}
}

bool AWyvernCharacter::IsWeakAttack(FName BoneName)
{
	if (Phase == EPhase::FirstPhase)
	{
		return BoneName.Compare(GetMesh()->GetSocketBoneName("Bip001_Head")) == 0;
	}
	else if (Phase == EPhase::SecondPhase)
	{
		return (BoneName.Compare(GetMesh()->GetSocketBoneName("BN_Wynern_Tail_1")) == 0) ||
			(BoneName.Compare(GetMesh()->GetSocketBoneName("BN_Wynern_Tail_5")) == 0);
	}
	else if (Phase == EPhase::ThirdPhase)
	{
		return false;
	}
	return false;
}

void AWyvernCharacter::CutTail(bool IsNotCut)
{
	if (HasAuthority())
	{
		if (!GetMesh()->DoesSocketExist("blindspot"))
		{
			UE_LOG(LogTemp, Warning, TEXT("No Socket exist : blindspot"));
			return;
		}
		FTransform Transform = GetMesh()->GetSocketTransform("blindspot");

		if (TailSurfaceClass)
		{
			TailSurface = Cast<AMySurface>(GetWorld()->SpawnActor(TailSurface->StaticClass()));
			TailSurface->AttachToComponent(
				GetMesh(),
				FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
				"blindspot"
			);
			if (IsNotCut)
			{
				FVector TailSpawnPoint = GetMesh()->GetSocketLocation("blindspot");
				FHitResult OutHit;
				TArray<AActor*> ActorsToIgnore;
				ActorsToIgnore.Add(this);

				if (UKismetSystemLibrary::LineTraceSingle(
					GetWorld(),
					TailSpawnPoint + FVector(0, 0, 30.0f),
					TailSpawnPoint + FVector(0, 0, -30.0f),
					UEngineTypes::ConvertToTraceType(ECC_Visibility),
					false,
					ActorsToIgnore,
					EDrawDebugTrace::None,
					OutHit,
					true
				)) {
					TailSpawnPoint = OutHit.ImpactPoint + FVector(0, 0, 100.0f);
				}

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride =
					ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				FTransform SpawnTransform(FRotator(0.f, 0.f, 0.f), TailSpawnPoint, FVector(1.0f, 1.0f, 1.0f));
				if (Tail)
				{
					GetWorld()->SpawnActor<AActor>(
						Tail,
						SpawnTransform,
						SpawnParams
					);
				}
			}
		}
	}
	FName TailBoneName = GetMesh()->GetSocketBoneName("BN_Wynern_Tail_5");
	GetMesh()->HideBoneByName(TailBoneName, EPhysBodyOp::PBO_None);
}

void AWyvernCharacter::DoDeath()
{
	GetController()->UnPossess();
	S2A_DoDeath();
}

void AWyvernCharacter::S2A_DoDeath_Implementation()
{
	DeadCollision();

	switch (Phase)
	{
	case EPhase::FirstPhase:
		if (HowlingMontage)
		{
			MonAIState = EAIState::RunawayReady;
			PlayAnimMontage(HowlingMontage);
		}
		break;

	case EPhase::SecondPhase:
		if (RevivalMontage)
		{
			MonAIState = EAIState::RunawayReady;
			PlayAnimMontage(RevivalMontage);
		}
		break;

	case EPhase::ThirdPhase:
		if (DeadMontage)
		{
			MonAIState = EAIState::Dead;
			PlayAnimMontage(DeadMontage);
			if (HasAuthority())
			{
				if (AInGameGameState* InGameGameState = GetWorld()->GetGameState<AInGameGameState>())
				{
					InGameGameState->SetCurrentGameState(EGameState::EGS_Win);
				}
			}
			APlayerCharacter* PlayerChar = Cast<APlayerCharacter>
				(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());
			if (PlayerChar)
			{
				PlayerChar->ChangeCameraToAnotherView(this, 5.0f);
			}
		}
		break;
	}

	
}

void AWyvernCharacter::DeadCollision()
{
	GetCharacterMovement()->DisableMovement();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // 지면만 막기

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWyvernCharacter::EventUpdateMonPhase(EPhase In_Phase)
{
	Phase = In_Phase;
}

void AWyvernCharacter::ShowMonsterHealthBar_Implementation(APlayerCharacter* InPlayer)
{
	APlayerController* OwnerPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (OwnerPC)
	{
		AMyHUD* LocalHud = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
		if (LocalHud)
		{
			if (!LocalHud->IsShowHealthBar)
			{
				LocalHud->ShowMonsterHealthBar();
				LocalHud->BindWyvernEvent(this);
			}
		}
	}
	// 단독으로 몬스터 체력 UI 노출
	//if (InPlayer->IsLocallyControlled())
	//{
	//	APlayerController* OwnerPC = Cast<APlayerController>(InPlayer->GetController());
	//	//OwnerPC->CallEnCounter(this);
	//	if (OwnerPC && OwnerPC == UGameplayStatics::GetPlayerController(GetWorld(), 0))
	//	{
	//		AMyHUD* LocalHud = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	//		if (LocalHud)
	//		{
	//			if (!LocalHud->IsShowHealthBar)
	//			{
	//				LocalHud->ShowMonsterHealthBar();
	//				LocalHud->BindWyvernEvent(this);
	//			}
	//		}
	//	}
	//}
}

void AWyvernCharacter::HideMonsterHealthBar_Implementation(APlayerCharacter* InPlayer)
{
	//APlayerController* OwnerPC = Cast<APlayerController>(InPlayer->GetController());
	//if (OwnerPC)
	//{
	//	AMyHUD* LocalHud = Cast<AMyHUD>(OwnerPC->GetHUD());

	//	if (LocalHud)
	//	{
	//		if (!LocalHud->IsShowHealthBar)
	//		{

	//			LocalHud->ShowMonsterHealthBar();
	//			AWyvernCharacter* WyvernCharacter = Cast<AWyvernCharacter>(GetPawn());
	//			if (WyvernCharacter)
	//			{
	//				//LocalHud->UnBindWyvernEvent(WyvernCharacter);
	//			}
	//		}
	//	}
	//}
}

bool AWyvernCharacter::IsMonsterMovable()
{
	bool IsMovable = false;
	switch (MonAIState)
	{
	case(EAIState::Patrol):
		IsMovable = true;
		break;

	case(EAIState::Chase):
		IsMovable = true;
		break;

	case(EAIState::Battle):
		IsMovable = false;
		break;

	case(EAIState::Dead):
		IsMovable = false;
		break;

	case(EAIState::Runaway):
		IsMovable = true;
		break;

	case(EAIState::RunawayReady):
		IsMovable = true;
		break;
	}
	return IsMovable;
}

void AWyvernCharacter::OnRep_Phase()
{
	switch (Phase)
	{
	case EPhase::FirstPhase:
		break;
	case EPhase::SecondPhase:
		break;
	case EPhase::ThirdPhase:
		break;
	}

}

void AWyvernCharacter::OnRep_MonAIState()
{
	switch (MonAIState)
	{
	case EAIState::Patrol:
		break;
	case EAIState::Chase:
		break;
	case EAIState::Battle:
		break;
	case EAIState::Dead:
		break;
	case EAIState::Runaway:
		break;
	}
}

void AWyvernCharacter::OnRep_Damage()
{

}

void AWyvernCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWyvernCharacter, MonAIState);
	DOREPLIFETIME(AWyvernCharacter, Phase);
}
