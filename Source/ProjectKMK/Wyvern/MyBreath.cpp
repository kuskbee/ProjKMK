// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBreath.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "../Interfaces/CombatReactInterface.h"

// Sets default values
AMyBreath::AMyBreath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	Particle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	Particle->SetupAttachment(SceneRoot);
	Particle->SetRelativeScale3D(FVector(2.5f, 2.5f, 2.5f));

	End = CreateDefaultSubobject<USphereComponent>(TEXT("End"));
	End->SetupAttachment(Particle);
	End->SetRelativeLocation(FVector(0, 0, 712.0f));
	End->SetRelativeRotation(FRotator(-90.0f, 180.0f, 180.0f));
	End->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));
	End->SetSphereRadius(32.0f);

	Start = CreateDefaultSubobject<USphereComponent>(TEXT("Start"));
	Start->SetupAttachment(Particle);
	Start->SetRelativeLocation(FVector(0, 0, 56.0f));
	Start->SetRelativeRotation(FRotator(-90.0f, 180.0f, 180.0f));
	Start->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));

}

// Called when the game starts or when spawned
void AMyBreath::BeginPlay()
{
	Super::BeginPlay();
	
	UKismetSystemLibrary::K2_SetTimer(
		this,
		"BreathDamage",
		0.5f,
		true,
		false
	);

	SetLifeSpan(1.85f);
}

// Called every frame
void AMyBreath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyBreath::BreathDamage()
{
	FVector StartLoc = Start->GetComponentLocation();
	FVector EndLoc = End->GetComponentLocation();
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetInstigator());


	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		GetWorld(),
		StartLoc,
		EndLoc,
		200.0f,
		300.0f,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHits,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		500.0f
	);

	for (struct FHitResult OutHit : OutHits)
	{
		if (OutHit.GetActor())
		{
			UGameplayStatics::ApplyDamage(
				OutHit.GetActor(),
				Damage,
				OutHit.GetActor()->GetInstigatorController(),
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
}

