// Fill out your copyright notice in the Description page of Project Settings.


#include "MonStateComponent.h"
#include "WyvernCharacter.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values for this component's properties
UMonStateComponent::UMonStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMonStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMonStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMonStateComponent::AddDamage(float Damage, FName BoneName, EPhase MonsterPhase)
{
	if (IsDeath(CurMonState))
		return;

	AWyvernCharacter* Character = Cast<AWyvernCharacter>(GetOwner());
	if (IsValid(Character))
	{
		USkeletalMeshComponent* Mesh = Character->GetMesh();
		if (MonsterPhase == EPhase::FirstPhase)
		{
			FName Head = Mesh->GetSocketBoneName(TEXT("Bip001_Head"));
			if (BoneName.Compare(Head) == 0)
			{
				CurMonState.CurHP = CurMonState.CurHP - Damage;
				CurMonState.CurWeakHP = CurMonState.CurWeakHP - Damage;
				EventDispatcherUpdateHP.Broadcast(
					CurMonState.MonData.MaxHP,
					CurMonState.MonData.WeaknessHP,
					CurMonState.CurHP,
					CurMonState.CurWeakHP
				);

				if (IsDeath(CurMonState))
				{
					EventDispatcherDeath.Broadcast();
				}
			}
			else
			{
				CurMonState.CurHP = CurMonState.CurHP - Damage;
				CurMonState.CurWeakHP = CurMonState.CurWeakHP;
				EventDispatcherUpdateHP.Broadcast(
					CurMonState.MonData.MaxHP,
					CurMonState.MonData.WeaknessHP,
					CurMonState.CurHP,
					CurMonState.CurWeakHP
				);

				if (IsDeath(CurMonState))
				{
					EventDispatcherDeath.Broadcast();
				}
			}
		}
		else
		{
			if (MonsterPhase == EPhase::SecondPhase)
			{
				FName Tail_1 = Mesh->GetSocketBoneName(TEXT("BN_Wynern_Tail_1"));
				FName Tail_5 = Mesh->GetSocketBoneName(TEXT("BN_Wynern_Tail_5"));
				if (BoneName.Compare(Tail_1) == 0 || BoneName.Compare(Tail_5) == 0)
				{
					CurMonState.CurHP = CurMonState.CurHP - Damage;
					CurMonState.CurWeakHP = CurMonState.CurWeakHP - Damage;
					EventDispatcherUpdateHP.Broadcast(
						CurMonState.MonData.MaxHP,
						CurMonState.MonData.WeaknessHP,
						CurMonState.CurHP,
						CurMonState.CurWeakHP
					);
					if (IsDeath(CurMonState))
					{
						EventDispatcherDeath.Broadcast();
					}
				}
				else
				{
					CurMonState.CurHP = CurMonState.CurHP - Damage;
					CurMonState.CurWeakHP = CurMonState.CurWeakHP;
					EventDispatcherUpdateHP.Broadcast(
						CurMonState.MonData.MaxHP,
						CurMonState.MonData.WeaknessHP,
						CurMonState.CurHP,
						CurMonState.CurWeakHP
					);
					if (IsDeath(CurMonState))
					{
						EventDispatcherDeath.Broadcast();
					}
				}
			}
			else
			{
				CurMonState.CurHP = CurMonState.CurHP - Damage;
				CurMonState.CurWeakHP = CurMonState.CurWeakHP;
				EventDispatcherUpdateHP.Broadcast(
					CurMonState.MonData.MaxHP,
					CurMonState.MonData.WeaknessHP,
					CurMonState.CurHP,
					CurMonState.CurWeakHP
				);

				if (IsDeath(CurMonState))
				{
					EventDispatcherDeath.Broadcast();
				}
			}
		}
	}
}

void UMonStateComponent::SetMonState(FName RowName)
{
	if (MonStateDataTable)
	{
		FMyMonState* Data = MonStateDataTable->FindRow<FMyMonState>(RowName, RowName.ToString());
		if (Data)
		{
			CurMonState.MonData.Name = Data->Name;
			CurMonState.MonData.MaxHP = Data->MaxHP;
			CurMonState.MonData.WeaknessHP = Data->WeaknessHP;
			CurMonState.MonData.Phase = Data->Phase;
			CurMonState.CurHP = Data->MaxHP;
			CurMonState.CurWeakHP = Data->WeaknessHP;
		}
	}
}

bool UMonStateComponent::IsDeath(FMyCurMonState State)
{
	return (State.CurHP <= 0 || State.CurWeakHP <= 0);
}

