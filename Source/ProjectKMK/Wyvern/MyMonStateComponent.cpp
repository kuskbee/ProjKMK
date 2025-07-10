// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMonStateComponent.h"
#include "WyvernCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UMyMonStateComponent::UMyMonStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void UMyMonStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMyMonStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

}

void UMyMonStateComponent::AddDamage(float Damage, FName BoneName, EPhase MonsterPhase)
{
	AWyvernCharacter* Character = Cast<AWyvernCharacter>(GetOwner());
	if (IsValid(Character))
	{
		USkeletalMeshComponent* Mesh = Character->GetMesh();

		switch (MonsterPhase)
		{
		case EPhase::FirstPhase:
			if (BoneName.Compare(Mesh->GetSocketBoneName(TEXT("Bip001_Head"))) == 0)
			{
				CurMonState.CurHP = CurMonState.CurHP - Damage;
				CurMonState.CurWeakHP = CurMonState.CurWeakHP - Damage;
			}
			else
			{
				CurMonState.CurHP = CurMonState.CurHP - Damage;
			}
			break;
		case EPhase::SecondPhase:
			if (BoneName.Compare(Mesh->GetSocketBoneName(TEXT("BN_Wynern_Tail_1"))) == 0 
				|| BoneName.Compare(Mesh->GetSocketBoneName(TEXT("BN_Wynern_Tail_5"))) == 0)
			{
				CurMonState.CurHP = CurMonState.CurHP - Damage;
				CurMonState.CurWeakHP = CurMonState.CurWeakHP - Damage;
			}
			else
			{
				CurMonState.CurHP = CurMonState.CurHP - Damage;
			}
			break;
		case EPhase::ThirdPhase:
			CurMonState.CurHP = CurMonState.CurHP - Damage;
			break;
		}

		S2A_AddDamage();
	}
}

void UMyMonStateComponent::S2A_AddDamage_Implementation()
{
	EventDispatcher_UpdateHP.Broadcast(
		CurMonState.MonData.MaxHP,
		CurMonState.MonData.WeaknessHP,
		CurMonState.CurHP,
		CurMonState.CurWeakHP
	);

	if (IsDeath())
	{
		EventDispatcher_Death.Broadcast();
	}
}

void UMyMonStateComponent::SetMonState(EPhase InPhase)
{
	if (MonStateDataTable)
	{
		FName RowName = MonStateDataTable->GetRowNames()[uint8(InPhase)];
		FMyMonState* Data = MonStateDataTable->FindRow<FMyMonState>(RowName, RowName.ToString());
		if (Data)
		{
			CurMonState.MonData.Name = Data->Name;
			CurMonState.MonData.MaxHP = Data->MaxHP;
			CurMonState.MonData.WeaknessHP = Data->WeaknessHP;
			CurMonState.CurHP = Data->MaxHP;
			CurMonState.CurWeakHP = Data->WeaknessHP;
		}
	}
}

bool UMyMonStateComponent::IsDeath()
{
	return (CurMonState.CurHP <= 0 || CurMonState.CurWeakHP <= 0);
}

void UMyMonStateComponent::OnRep_CurMonState()
{
	EventDispatcher_UpdateHP.Broadcast(
		CurMonState.MonData.MaxHP,
		CurMonState.MonData.WeaknessHP,
		CurMonState.CurHP,
		CurMonState.CurWeakHP
	);
}

void UMyMonStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMyMonStateComponent, CurMonState);

}

