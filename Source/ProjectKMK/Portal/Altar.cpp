// Fill out your copyright notice in the Description page of Project Settings.


#include "Altar.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Portal.h"
#include "../PlayerCharacter.h" // TEST

// Sets default values
AAltar::AAltar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AltarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AltarMesh"));
	AltarMesh->SetupAttachment(GetRootComponent());

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(AltarMesh);

	//
	AltarMesh->SetRelativeScale3D(FVector(4.f, 4.f, 0.25f));
	Box->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
	Box->SetRelativeScale3D(FVector(2.f, 2.f, 10.f));

}

// Called when the game starts or when spawned
void AAltar::BeginPlay()
{
	Super::BeginPlay();
	
	Box->OnComponentBeginOverlap.AddDynamic(this, &AAltar::OnBoxBeginOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this, &AAltar::OnBoxEndOverlap);
}

void AAltar::ShowOpenEffect()
{
	if (IsValid(ParticleSystem))
	{
		ParticleSystem->DeactivateSystem();
		ParticleSystem->DestroyComponent();
		ParticleSystem = nullptr;
	}

	if (!OpenEffectTemplate)
	{
		return;
	}

	ParticleSystem = UGameplayStatics::SpawnEmitterAttached(
		OpenEffectTemplate,
		GetRootComponent(),               // 붙일 부모
		NAME_None,
		FVector::ZeroVector,              // 상대 위치
		FRotator::ZeroRotator,
		FVector(2.f),                     // 상대 스케일
		EAttachLocation::KeepRelativeOffset,
		/*bAutoDestroy=*/true             // 끝나면 자동으로 컴포넌트 제거
	);

}

// Called every frame
void AAltar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bool IsCountDown = IsOverlappedTail && !IsTriggerOpen;
	if (IsCountDown)
	{
		CurOverlappedSeconds += DeltaTime;

		if (CurOverlappedSeconds >= OverlappedThresholdSeconds)
		{
			IsTriggerOpen = true;

			if (IsValid(LevelMovePortal))
			{
				bool bIsHidden = LevelMovePortal->IsHidden();
				if (bIsHidden)
				{
					LevelMovePortal->EnterOpening(this);
					UE_LOG(LogTemp, Warning, TEXT("Trigger Box! - Open Portal"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[Warning] Please Set Value of LevelMovePortal!"));
			}
		}
	}
}

void AAltar::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*ATail* Tail = Cast<ATail>(OtherActor);
	if(IsValid(Tail))
	{
		IsOverlappedTail = true;
		CurOverlappedSeconds = 0.f;
	}*/

	//  TEST
	APlayerCharacter* Tail = Cast<APlayerCharacter>(OtherActor);
	if (IsValid(Tail))
	{
		IsOverlappedTail = true;
	}
}

void AAltar::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*ATail* Tail = Cast<ATail>(OtherActor);
	if(IsValid(Tail))
	{
		IsOverlappedTail = false;
	}*/

	// TEST
	APlayerCharacter* Tail = Cast<APlayerCharacter>(OtherActor);
	if (IsValid(Tail))
	{
		IsOverlappedTail = false;
		CurOverlappedSeconds = 0.f;
	}
}
