// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "../Interfaces/CombatReactInterface.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	// * Initialize Components
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponMesh->SetIsReplicated(true);

	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(WeaponMesh);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceStart"));
	BoxTraceStart->SetupAttachment(WeaponMesh);

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceEnd"));
	BoxTraceEnd->SetupAttachment(WeaponMesh);

	// * Initialize Default Values
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);

	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

	WeaponBox->SetRelativeLocation(FVector(0.f, 0.f, 54.5f));
	WeaponBox->SetBoxExtent(FVector(1.7f, 1.7f, 35.f));

	BoxTraceStart->SetRelativeLocation(FVector(0.f, 0.f, 10.f));
	BoxTraceEnd->SetRelativeLocation(FVector(0.f, 0.f, 86.f));
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	//WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnWeaponBoxOverlap);

	AActor* IgnoreActor = Cast<AActor>(OwnerCharacter);
	WeaponBox->IgnoreActorWhenMoving(IgnoreActor, true);

	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ACharacter>(GetOwner());
	}
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponBase::SetWeaponCollisionEnable(bool IsEnable)
{
	// 공격 방식을 플레이어에서 직접 공격 방식으로 변경하여 주석처리
	/*if (IsEnable)
	{
		WeaponBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ClearIgnoreActors();
	}*/
}

void AWeaponBase::ClearIgnoreActors()
{
	IgnoreActors.Reset();
}

bool AWeaponBase::HasSameTag(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return false;
	}

	for (const FName& Tag : OwnerCharacter->Tags)
	{
		if (TargetActor->Tags.Contains(Tag))
		{
			return true;
		}
	}

	return false;
}

bool AWeaponBase::DoBoxTrace(AActor* TargetActor, FHitResult& OutHit)
{
	TArray<TObjectPtr<AActor>> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(OwnerCharacter);

	for (TObjectPtr<AActor> Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	FVector Start = BoxTraceStart->GetComponentLocation();
	FVector End = BoxTraceEnd->GetComponentLocation();
	FVector HalfSize(17.f, 17.f, 17.f);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2));

	bool bHit = UKismetSystemLibrary::BoxTraceSingleForObjects(this, Start, End, HalfSize,
		BoxTraceStart->GetComponentRotation(),
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		OutHit,
		true
	);

	UE_LOG(LogTemp, Error, TEXT("[DoBoxTrace] Target:%s, bHit : %d"), *TargetActor->GetName(), bHit);

	return bHit;
}

bool AWeaponBase::HasSameMonsterTag(AActor* TargetActor)
{
	if (!TargetActor || !OwnerCharacter)
	{
		return false;
	}

	bool bTargetHasTag = TargetActor->ActorHasTag("Monster");
	bool bOwnerHasTag = OwnerCharacter->ActorHasTag("Monster");
	return (bTargetHasTag && bOwnerHasTag);
}

// => 공격 방식 변경(Animation Notify : PlayerAttack -> PlayerCharacter에서 공격하도록 변경)
//void AWeaponBase::OnWeaponBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	//서버에서만 처리
//	if (!HasAuthority()) return;
//
//	UE_LOG(LogTemp, Warning, TEXT("[Weapon][Server] Overlap hit on: %s"), *OtherActor->GetName());
//	// 몬스터끼리 데미지받는 상황 막음
//	if (HasSameMonsterTag(OtherActor))
//	{
//		return;
//	}
//
//	FHitResult HitResult;
//	bool bHit = DoBoxTrace(OtherActor, HitResult);
//	if (bHit)
//	{
//		TObjectPtr<AActor> DamagedActor = HitResult.GetActor();
//
//		if (DamagedActor != OtherActor)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("[Weapon][Server] After DoBoxTrace %s / %s"), *OtherActor->GetName(), *DamagedActor->GetName() );
//			return;
//		}
//
//		// 몬스터끼리 데미지받는 상황 막음
//		if (HasSameMonsterTag(DamagedActor)) 
//		{
//			return;
//		}
//
//		if (IsValid(OwnerCharacter))
//		{
//			IgnoreActors.AddUnique(DamagedActor);
//
//			UGameplayStatics::ApplyPointDamage(DamagedActor, AttackPower, HitResult.ImpactPoint, HitResult,
//				OwnerCharacter->GetController(), this, UDamageType::StaticClass());
//
//			ICombatReactInterface* Hittable = Cast<ICombatReactInterface>(DamagedActor);
//			if (Hittable)
//			{
//				Hittable->ApplyHit(HitResult, OwnerCharacter);
//			}
//		}
//	}
//}

