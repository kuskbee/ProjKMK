// Fill out your copyright notice in the Description page of Project Settings.


#include "WyvernCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionWarpingComponent.h"
#include "MonStateComponent.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "MyLegacyCameraShake.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
AWyvernCharacter::AWyvernCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->AddRelativeLocation(FVector(0.0f, 0.0f, -500.0f));
	GetMesh()->SetWorldScale3D(FVector(0.3f, 0.3f, 0.3f));

	GetCapsuleComponent()->SetCapsuleSize(500.0f, 500.0f);
	GetCapsuleComponent()->SetWorldScale3D(FVector(0.7f, 0.7f, 0.7f));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetCapsuleComponent());

	SpringArm->AddRelativeLocation(FVector(-3.158533, 25.579432, -52.963234));
	SpringArm->AddRelativeRotation(FRotator(-20.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 1200.0f;

	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("DeadCamera"));
	Camera->SetupAttachment(SpringArm);

	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));

	MonStateComponent = CreateDefaultSubobject<UMonStateComponent>(TEXT("MonStateComponent"));
}

// Called when the game starts or when spawned
void AWyvernCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWyvernCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AWyvernCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AWyvernCharacter::Attack()
{
	switch (Phase)
	{
	case EPhase::FirstPhase:
		BattleTickOnFirstPhase();
		break;
	case EPhase::SecondPhase:
		BattleTickOnSecondPhase();
		break;
	case EPhase::ThirdPhase:
		BattleTickOnThirdPhase();
		break;
	default:
		break;
	}

	return true;
}

bool AWyvernCharacter::SetAIState(EAIState AIState)
{
	MonAIState = AIState;

	return false;
}

bool AWyvernCharacter::ApplyHit(FHitResult HitResult, AActor* HitterActor)
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

		if (HitMontage)
		{
			PlayAnimMontage(HitMontage);
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
			//UNiagaraFunctionLibrary::SpawnSystemAtLocation(HitResult.ImpactPoint) 
			// 스폰할 나이아가라 시스템 리소스는 유틸리티로 찾게 두면 안되기 때문에 멤버 변수로 슬롯을 만들어
			// 블루프린트에서 찾아 넣을 수 있도록 할건데 
			// 오늘은 여기까지
		}
		else
		{

		}


		return true;
	}
	else
	{
		return false;
	}
}

void AWyvernCharacter::BattleTickOnFirstPhase()
{
}

void AWyvernCharacter::BattleTickOnSecondPhase()
{
}

void AWyvernCharacter::BattleTickOnThirdPhase()
{
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
