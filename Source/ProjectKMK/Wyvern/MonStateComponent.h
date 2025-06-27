// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "MyMonState.h"
#include "MyCurMonState.h"

#include "MonStateComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventDispatcher_MonState, FMyCurMonState, Monster);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FEventDispatcher_UpdateHP, float, MaxHP, float, WeaknessHP, float, CurHP, float, CurWeakHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEventDispatcher_Death);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class PROJECTKMK_API UMonStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMonStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	struct FMyCurMonState CurMonState;

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	EPhase Phase;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher", BlueprintCallable)
	FEventDispatcher_MonState EventDispatcher_MonState;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher", BlueprintCallable)
	FEventDispatcher_UpdateHP EventDispatcher_UpdateHP;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher", BlueprintCallable)
	FEventDispatcher_Death EventDispatcher_Death;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UDataTable> MonStateDataTable;

	UFUNCTION(BlueprintCallable)
	void AddDamage(float Damage, FName BoneName, EPhase MonsterPhase);

	UFUNCTION(BlueprintCallable)
	void SetMonState(FName RowName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsDeath(FMyCurMonState State);
};
