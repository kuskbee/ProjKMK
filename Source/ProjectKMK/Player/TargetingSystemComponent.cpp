// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingSystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "../PlayerCharacter.h"

// Sets default values for this component's properties
UTargetingSystemComponent::UTargetingSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTargetingSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (GetOwner())
	{
		APlayerCharacter* Owner = Cast<APlayerCharacter>(GetOwner());
		if (Owner)
		{
			Initialize(Owner, Owner->FollowCamera);
		}
	}
	
}


// Called every frame
void UTargetingSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 타겟이 있으면 타겟 방향으로 카메라 방향 고정시키기
	bool bTargeting = IsValid(LockedOnTarget) && IsValid(OwnerCharacter) && IsValid(Camera);
	if (bTargeting)
	{
		FVector PlayerLoc = OwnerCharacter->GetActorLocation();
		FVector TargetLoc = LockedOnTarget->GetActorLocation();
		
		FRotator Current = OwnerCharacter->GetControlRotation();
		FRotator Target = UKismetMathLibrary::FindLookAtRotation(PlayerLoc, TargetLoc);

		FRotator InterRotator = UKismetMathLibrary::RInterpTo(Current, Target,
			UGameplayStatics::GetWorldDeltaSeconds(GetWorld()),
			5.0f);
		
		FRotator TargetRotator = FRotator(Current.Pitch, InterRotator.Yaw, InterRotator.Roll);
		if (OwnerCharacter->GetController())
		{
			OwnerCharacter->GetController()->SetControlRotation(TargetRotator);
		}
	}
}

void UTargetingSystemComponent::Initialize(ACharacter* Owner, UCameraComponent* CameraComponent)
{
	OwnerCharacter = Cast<ACharacter>(Owner);
	Camera = CameraComponent;
}

TArray<AActor*> UTargetingSystemComponent::FindAllTargets()
{
	FVector Start = OwnerCharacter->GetActorLocation();
	FVector End = Start;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2));

	TArray<TObjectPtr<AActor>> IgnoreActors;
	IgnoreActors.Add(OwnerCharacter);

	TArray<FHitResult> OutHits;

	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Start, End,
		TraceDistance,
		ObjectTypes,
		false,
		IgnoreActors,
		EDrawDebugTrace::ForDuration,
		OutHits,
		true
	);

	for (FHitResult Hit : OutHits)
	{
		AActor* Target = Hit.GetActor();
		if (!Target)
		{
			continue;
		}

		bool HasTag = Target->ActorHasTag(FName(TargetTag));
		if (HasTag)
		{
			TargetEnemies.AddUnique(Target);
		}
	}

	return TargetEnemies;
}

AActor* UTargetingSystemComponent::LockOnToTarget()
{
	LockedOnTarget = nullptr;
	float ClosestDistance = TraceDistance;
	if (!IsValid(Camera))
	{
		return nullptr;
	}
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetForwardVector() * TraceDistance;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2));
	TArray<TObjectPtr<AActor>> IgnoreActors;
	IgnoreActors.Add(OwnerCharacter);

	TArray<FHitResult> OutHits;

	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Start, End,
		RadiusToLockOn,
		ObjectTypes,
		false,
		IgnoreActors,
		EDrawDebugTrace::ForDuration,
		OutHits,
		true,
		FLinearColor::Red,
		FLinearColor::Blue
	);

	for (int i = 0; i < OutHits.Num(); i++)
	{
		AActor* HitActor = OutHits[i].GetActor();
		if (false == TargetEnemies.Contains(HitActor))
		{
			continue;
		}

		float DistanceToOwner = HitActor->GetDistanceTo(OwnerCharacter);
		if (DistanceToOwner <= ClosestDistance)
		{
			ClosestDistance = DistanceToOwner;
			LockedOnTarget = HitActor;
			LockedOnTargetIndex = i;
		}
	}

	if (IsValid(LockedOnTarget))
	{
		OnLockOnNewTarget.Broadcast();
	}

	return LockedOnTarget;
}

void UTargetingSystemComponent::ResetLockedOnTarget()
{
	if (IsValid(LockedOnTarget))
	{
		// * Indicator 처리할 거면 여기서 visibility : false

		// 초기화
		LockedOnTarget = nullptr;
		TargetEnemies.Empty();
		LockedOnTargetIndex = 0;
	}
	OnResetLockedOnTarget.Broadcast();
}

void UTargetingSystemComponent::ChangeTarget()
{
	bool IsValidVariables = TargetEnemies.IsValidIndex(0) && IsValid(LockedOnTarget);

	if (IsValidVariables)
	{
		//  타겟이 둘 이상일 때만 타겟 변경
		if (TargetEnemies.Num() > 1)
		{
			// * indicator 처리를 한다면 
			// * 여기서 TargetEnemies를 순회하면서 안보이도록 처리한다.

			if (LockedOnTargetIndex >= TargetEnemies.Num() - 1)
			{
				LockedOnTargetIndex = 0;
			}
			else
			{
				LockedOnTargetIndex += 1;
			}

			TargetEnemies[LockedOnTargetIndex] = LockedOnTarget;
			// * Indicator 설정 하기

			OnLockOnNewTarget.Broadcast();
		 }
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Target Enemies are invalid!"));
	}
}