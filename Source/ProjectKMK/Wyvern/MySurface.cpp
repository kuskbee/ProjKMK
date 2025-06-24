// Fill out your copyright notice in the Description page of Project Settings.


#include "MySurface.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
AMySurface::AMySurface()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	Tail = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tail"));
	Tail->SetupAttachment(RootComponent);

	Tail->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	Tail->SetRelativeScale3D(FVector(0.52f, 0.52f, 0.52f));
}

// Called when the game starts or when spawned
void AMySurface::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMySurface::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

