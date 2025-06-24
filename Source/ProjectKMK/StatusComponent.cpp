// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusComponent.h"

// Sets default values for this component's properties
UStatusComponent::UStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UStatusComponent::InitializeStatus()
{
	CurHp = MaxHp;
	UpdateUIHp();
}

void UStatusComponent::TakeDamage(float Damage)
{
	CurHp = FMath::Clamp(CurHp - Damage, 0.f, MaxHp);
	
	UpdateUIHp();

	if (IsDeath())
	{
		OnDead.Broadcast();

		//:HUD: ChangeGameState Lose
	}
}

void UStatusComponent::UpdateUIHp()
{
	OnChangeHp.Broadcast(CurHp, MaxHp);
}

bool UStatusComponent::IsDeath()
{
	return (CurHp <= 0.f);
}

// Called when the game starts
void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeStatus();
}
