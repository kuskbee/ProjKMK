// Fill out your copyright notice in the Description page of Project Settings.


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

	SetMonState(Phase);

	MonStateComponent->EventDispatcher_Death.AddDynamic(this,
		&AWyvernCharacter::DoDeath);

	OnTakePointDamage.AddDynamic(this,
		&AWyvernCharacter::EventProcessTakePointDamage);
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
				TailSurface->K2_DestroyActor();
			}
			K2_DestroyActor();
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

	InitAI(NewController);
}

void AWyvernCharacter::Attack()
{
	if (!IsPlayMontage)
	{
		float PlayerRate;
		TArray<FName> Names;
		int Random;
		FName RandName;
		FST_MyMonsterSkill* Skill;

		switch (Phase)
		{
		case EPhase::FirstPhase:
			if (FirstPhaseTable)
			{
				PlayerRate = 1.2f;
				Names = FirstPhaseTable->GetRowNames();
				Random = FMath::RandRange(0, Names.Num() - 1);
				RandName = Names[Random];
				Skill = FirstPhaseTable->FindRow<FST_MyMonsterSkill>(RandName, RandName.ToString());
				AttackMontage = Skill->SkillAnimMontage;
			}

			break;

		case EPhase::SecondPhase:
			if (SecondPhaseTable)
			{
				PlayerRate = 1.2f;
				Names = SecondPhaseTable->GetRowNames();
				Random = FMath::RandRange(0, Names.Num() - 1);
				RandName = Names[Random];
				Skill = SecondPhaseTable->FindRow<FST_MyMonsterSkill>(RandName, RandName.ToString());
				AttackMontage = Skill->SkillAnimMontage;
			}

			break;
		case EPhase::ThirdPhase:
			if (ThirdPhaseTable)
			{
				PlayerRate = 1.4f;
				Names = ThirdPhaseTable->GetRowNames();
				Random = FMath::RandRange(0, Names.Num() - 1);
				RandName = Names[Random];
				Skill = ThirdPhaseTable->FindRow<FST_MyMonsterSkill>(RandName, RandName.ToString());
				AttackMontage = Skill->SkillAnimMontage;
			}
			break;
		}
		if (AttackMontage)
		{
			S2A_OnAttack(AttackMontage, 1.4f);
		}
	}
}

//void AWyvernCharacter::C2S_Attack_Implementation()
//{
//
//}
//
//bool AWyvernCharacter::C2S_Attack_Validate()
//{
//
//	return false;
//}

void AWyvernCharacter::S2A_OnAttack_Implementation(UAnimMontage* InAttackMontage, float InPlayerRate)
{
	if (InAttackMontage)
	{
		PlayAnimMontage(InAttackMontage, InPlayerRate);

		if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(InAttackMontage))
		{
			IsPlayMontage = true;
		}
		else
		{
			EventMontageEnd(InAttackMontage, false);
		}
	}
}

bool AWyvernCharacter::ApplyHit(const FHitResult& HitResult, AActor* HitterActor)
{
	if (MonAIState != EAIState::Dead && MonAIState != EAIState::Runaway && MonAIState != EAIState::RunawayReady)
	{
		if (FMath::FRandRange(0.0f, 1.0f) <= 0.7f)
		{
			float RandomKnockbackPower = FMath::FRandRange(300.0f, 600.0f);

			MotionWarping->AddOrUpdateWarpTargetFromLocation(
				FName("MonsterKnockbackMove"),
				GetActorLocation() + ((GetActorLocation() - HitterActor->GetActorLocation()).Normalize(0.0001)
					* RandomKnockbackPower));
		}
		else
		{
			MotionWarping->RemoveAllWarpTargets();
		}

		if (KnockBackMontage)
		{
			PlayAnimMontage(KnockBackMontage);
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

		return true;
	}
	else
	{
		return false;
	}
}

void AWyvernCharacter::SetMonState(EPhase InPhase)
{
	if (IsValid(MonStateComponent))
	{
		MonStateComponent->SetMonState(InPhase);
	}
}

void AWyvernCharacter::CutTail(bool IsNotCut)
{
	if (!GetMesh()->DoesSocketExist("blindspot"))
	{
		UE_LOG(LogTemp, Warning, TEXT("No Socket exist : blindspot"));
		return;
	}
	FTransform Transform = GetMesh()->GetSocketTransform("blindspot");

	if (TailSurface)
	{
		AMySurface* Surface = Cast<AMySurface>(GetWorld()->SpawnActor(TailSurface->StaticClass()));
		Surface->AttachToComponent(
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
			)){
				TailSpawnPoint = OutHit.ImpactPoint + FVector(0, 0, 100.0f);
			}

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = 
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			FTransform SpawnTransform(FRotator(0.f, 0.f, 0.f), TailSpawnPoint, FVector(1.0f, 1.0f, 1.0f));
			GetWorld()->SpawnActor<AActor>(
				TailSurface->StaticClass(),
				SpawnTransform,
				SpawnParams
			);
				
		}
	}
}

void AWyvernCharacter::EventMontageEnd(UAnimMontage* Montage, bool bINterrupted)
{
	IsPlayMontage = false;
	if (Montage == AttackMontage)
	{
		EventAttackEnd.Broadcast();
	}
}

void AWyvernCharacter::EventUpdateMonPhase(EPhase In_Phase)
{
	Phase = In_Phase;
}

void AWyvernCharacter::EventProcessTakePointDamage(AActor* DamagedActor, float In_Damage,
	AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, 
	const UDamageType* DamageType, AActor* DamageCauser)
{
	if (MonAIState == EAIState::Patrol)
	{
		AMyMonAIController* MonController = Cast<AMyMonAIController>(GetController());
		if (MonController)
		{
			//MonController->FirstEncounterTarget(InstigatedBy);
//			나중에 BT 전부 완성되면 ShowMonsterHealthBar() 를 WyvernCharacter 쪽으로 옮길거임
			//if (Phase == EPhase::ThirdPhase)
			//{
			//	ShowMonsterHealthBar();
			//}
		}
	}

	MonStateComponent->AddDamage(In_Damage, BoneName, Phase);

}

void AWyvernCharacter::DoAttack(bool IsRightHand, bool IsMouth)
{
	FVector AttackLocation;
	if (IsRightHand)
	{
		AttackLocation = GetMesh()->GetSocketLocation("rhand_socket");
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

	FHitResult OutHit;
	UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		AttackLocation,
		AttackLocation,
		250.0f,
		ObjectTypes,
		false,
		ActorToIgnore,
		EDrawDebugTrace::None,
		OutHit,
		true
	);

	if (OutHit.GetActor())
	{
		UGameplayStatics::ApplyDamage(
			OutHit.GetActor(),
			Damage,
			GetController(),
			this,
			NULL
		);

		ICombatReactInterface* Object = Cast<ICombatReactInterface>(OutHit.GetActor());
		if (Object)
		{
			Object->ApplyHit(OutHit, this);
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

void AWyvernCharacter::InitAI(UObject* In_Controller)
{
	AMyMonAIController* MonAIController = Cast<AMyMonAIController>(In_Controller);

	if (MonAIController)
	{
		GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this,
			&AWyvernCharacter::EventMontageEnd);
	}
}

void AWyvernCharacter::UpdateWalkSpeed(float In_WalkSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = In_WalkSpeed;
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

void AWyvernCharacter::DoDeath()
{
	DeadCollision();

	switch (Phase)
	{
	case EPhase::FirstPhase:
		if (HowlingMontage)
		{
			MonAIState = EAIState::RunawayReady;
			PlayAnimMontage(HowlingMontage);
			IsPlayMontage = true;
		}

		break;

	case EPhase::SecondPhase:
		if (RevivalMontage)
		{
			MonAIState = EAIState::RunawayReady;
			PlayAnimMontage(RevivalMontage);
			IsPlayMontage = true;
		}

		break;

	case EPhase::ThirdPhase:
		if (DeadMontage)
		{
			MonAIState = EAIState::Dead;
			PlayAnimMontage(DeadMontage);

			AMyHUD* LocalHud = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
		
			if (LocalHud)
			{
				LocalHud->EventChangeGameState(EGameState::EGS_Win);
			}
		}

		break;
	}

	APlayerCharacter* PlayerChar = Cast<APlayerCharacter>
		(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());
	if (PlayerChar)
	{
		//PlayerChar->ChangeCameraToAnotherView(); 아직 미구현 한준님 담당!
	}
}

void AWyvernCharacter::DeadCollision()
{
	AController* MyController = GetController();
	if (MyController)
	{
		MyController->UnPossess();

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
