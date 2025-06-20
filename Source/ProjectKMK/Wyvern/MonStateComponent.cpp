// Fill out your copyright notice in the Description page of Project Settings.


#include "MonStateComponent.h"

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

}

void UMonStateComponent::SetMonState(FName RowName)
{
	if (MonStateDataTable)
	{
		FMonState* Data = MonStateDataTable->FindRow<FMonState>(RowName, RowName.ToString());
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

bool UMonStateComponent::IsDeath(FCurMonState State)
{
	return (State.CurHP <= 0 || State.CurWeakHP <= 0);
}

