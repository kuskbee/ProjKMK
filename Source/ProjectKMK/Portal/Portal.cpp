// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "../PlayerCharacter.h"
#include "Altar.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(GetRootComponent());

	// * Initialize Default Values
	Box->SetGenerateOverlapEvents(true);
	Box->SetRelativeScale3D(FVector(2.f));
	Box->SetRelativeLocation(FVector(20.f, 0.f, 0.f));
	Box->SetBoxExtent(FVector(15.f, 42.f, 65.f));

	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();

	Box->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnBoxBeginOverlap);
	
	HidePortal();
}

void APortal::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* OverlappedPawn = Cast<APawn>(OtherActor);
	if (IsValid(OverlappedPawn))
	{
		MoveNextLevel(OverlappedPawn);
	}
}

void APortal::OnOpeningEffectFinished(UParticleSystemComponent* PSystem)
{
	if (PortalState == EPortalState::EPS_Opening)
	{
		EnterOpen();
	}
}

void APortal::HidePortal()
{
	PortalState = EPortalState::EPS_Hidden;
	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PlayPortalVFX(EPortalState::EPS_Opening, false); // 끔
	PlayPortalVFX(EPortalState::EPS_Open, false);
	PlayPortalVFX(EPortalState::EPS_Closing, false);
}

void APortal::EnterOpening(AAltar* _Altar)
{
	Altar = _Altar;
	PortalState = EPortalState::EPS_Opening;

	PlayPortalVFX(EPortalState::EPS_Opening, true);

	// :MULTI:  멀티로 가면 바뀌어야함!
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
	if (IsValid(PC))
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(PC->GetPawn());
		if (IsValid(Player) && IsValid(PortalCamera))
		{
			Player->ChangeCameraToAnotherView(PortalCamera, 3.0f);
		}
	}
}

bool APortal::IsHidden()
{
	return (PortalState == EPortalState::EPS_Hidden);
}

void APortal::EnterOpen()
{
	PortalState = EPortalState::EPS_Open;
	PlayPortalVFX(EPortalState::EPS_Opening, false);
	PlayPortalVFX(EPortalState::EPS_Open, true);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	if (IsValid(Altar))
	{
		Altar->ShowOpenEffect();
	}
}

void APortal::MoveNextLevel(APawn* Target)
{
	// :MULTI: 멀티면 고쳐야한다.
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (IsValid(PC) && IsValid(PC->GetPawn()))
	{
		// Overlapped Pawn이 플레이어인지 체크
		if (PC->GetPawn() == Target)
		{
			if (!NextLevelName.IsEmpty())
			{
				UGameplayStatics::OpenLevel(GetWorld(), FName(NextLevelName));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[Warning] Map Name is Empty!!!"));
			}
		}
	}
}

void APortal::EnterClosing()
{
	HidePortal();
	PortalState = EPortalState::EPS_Closing;
	PlayPortalVFX(EPortalState::EPS_Closing, true);
}

void APortal::PlayPortalVFX(EPortalState Type, bool bSpawn)
{
	// 이전에 재생중이던 파티클이 있으면 끄고 파괴
	if (IsValid(CurrentPSC))
	{
		CurrentPSC->DeactivateSystem();
		CurrentPSC->DestroyComponent();
		CurrentPSC = nullptr;
	}

	if (!bSpawn)
	{
		return;
	}

	UParticleSystem* Template = nullptr;
	switch (Type)
	{
	case EPortalState::EPS_Opening:  Template = VFX_OpeningTemplate;  break;
	case EPortalState::EPS_Open:     Template = VFX_OpenTemplate;     break;
	case EPortalState::EPS_Closing:  Template = VFX_ClosingTemplate;  break;
	default:                   break;
	}
	if (!Template)
	{
		return;
	}

	// 한 번만 재생할 파티클을 Attach + AutoDestroy 로 Spawn
	CurrentPSC = UGameplayStatics::SpawnEmitterAttached(
		Template,
		GetRootComponent(),               // 붙일 부모
		NAME_None,
		FVector::ZeroVector,              // 상대 위치
		FRotator::ZeroRotator,
		FVector(2.f),                     // 상대 스케일
		EAttachLocation::KeepRelativeOffset,
		/*bAutoDestroy=*/true             // 끝나면 자동으로 컴포넌트 제거
	);

	if (Type == EPortalState::EPS_Opening && IsValid(CurrentPSC))
	{
		CurrentPSC->OnSystemFinished.AddDynamic(
			this, &APortal::OnOpeningEffectFinished);
	}
}