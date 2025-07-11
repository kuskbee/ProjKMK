// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyMonState.h"
#include "MyCurMonState.h"
#include "MyDefine.h"
#include "MyMonStateComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FEventDispatcher_UpdateHP, float, MaxHP, float, WeaknessHP, float, CurHP, float, CurWeakHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEventDispatcher_Death);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class PROJECTKMK_API UMyMonStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyMonStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(ReplicatedUsing=OnRep_CurMonState, EditAnywhere, Category = "Data", BlueprintReadWrite)
	struct FMyCurMonState CurMonState;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher", BlueprintCallable)
	FEventDispatcher_UpdateHP EventDispatcher_UpdateHP;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher", BlueprintCallable)
	FEventDispatcher_Death EventDispatcher_Death;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UDataTable> MonStateDataTable;

	UFUNCTION()
	void AddDamage(float Damage, FName BoneName, EPhase MonsterPhase);

	UFUNCTION(NetMulticast, Reliable)
	void S2A_AddDamage();
	void S2A_AddDamage_Implementation();


	UFUNCTION(BlueprintCallable)
	void SetMonState(EPhase InPhase);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsDeath();

	UFUNCTION()
	void OnRep_CurMonState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
