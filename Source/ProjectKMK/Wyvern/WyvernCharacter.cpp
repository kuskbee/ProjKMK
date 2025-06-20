// Fill out your copyright notice in the Description page of Project Settings.


#include "WyvernCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionWarpingComponent.h"
#include "MonStateComponent.h"

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

