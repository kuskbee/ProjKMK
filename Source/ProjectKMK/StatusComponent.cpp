// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/MyHUD.h"
#include "Kismet/GameplayStatics.h"
#include "InGameGameState.h"

// Sets default values for this component's properties
UStatusComponent::UStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	MaxHp = 1000.0f;
	CurHp = MaxHp;
	bIsDead = false;
	// ...
}

void UStatusComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStatusComponent, CurHp);
	DOREPLIFETIME(UStatusComponent, bIsDead);
}

void UStatusComponent::InitializeStatus()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		CurHp = MaxHp;
		bIsDead = false;

		OnChangeHp.Broadcast(CurHp, MaxHp);
	}
}

void UStatusComponent::TakeDamage(float Damage)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		SetHp(CurHp - Damage);
	}
}

void UStatusComponent::UpdateUIHp()
{
	OnChangeHp.Broadcast(CurHp, MaxHp);
}

void UStatusComponent::SetHp(float NewHp)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		float OldHp = CurHp;
		CurHp = FMath::Clamp(NewHp, 0.f, MaxHp);

		if (CurHp != OldHp)
		{
			OnChangeHp.Broadcast(CurHp, MaxHp);
		}

		if (CalculateIsDead() && !bIsDead)
		{
			SetIsDead(true);
		}
		else if (!CalculateIsDead() && bIsDead)
		{
			SetIsDead(false);
		}
	}
}

void UStatusComponent::SetIsDead(bool bNewDeadStatus)
{
	if (GetOwner() && GetOwner()->HasAuthority()) //서버만 처리
	{
		bIsDead = bNewDeadStatus;
		OnDead.Broadcast(bIsDead);

		if (bIsDead)
		{
			if (AInGameGameState* GS = GetWorld()->GetGameState<AInGameGameState>())
			{
				GS->SetCurrentGameState(EGameState::EGS_Lose);
			}
		}
		else
		{
			//Revive
		}
	}
}

void UStatusComponent::OnRep_CurHp()
{
	UpdateUIHp();
}

void UStatusComponent::OnRep_IsDead()
{
	OnDead.Broadcast(bIsDead);
}

// Called when the game starts
void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		InitializeStatus();
	}
	else
	{
		UpdateUIHp();
	}
}
