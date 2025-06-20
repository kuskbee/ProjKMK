// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MonStateComponent.generated.h"

UENUM(BlueprintType)
enum class EPhase : uint8
{
	FirstPhase = 0 UMETA(DisplayName = "FirstPhase"),
	SecondPhase = 1 UMETA(DisplayName = "SecondPhase"),
	ThirdPhase = 2 UMETA(DisplayName = "ThirdPhase")
};


USTRUCT(BlueprintType)
struct FMonState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	float MaxHP;

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	float WeaknessHP;

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	EPhase Phase;

	FMonState()
	{
		Name = "",
		MaxHP = 10000.0f;
		WeaknessHP = 2000.0f;
		Phase = EPhase::FirstPhase;
	}
};

USTRUCT(BlueprintType)
struct FCurMonState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	FMonState MonData;

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	float CurHP;

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	float CurWeakHP;

	FCurMonState()
	{
		MonData.Name = "";
		MonData.MaxHP = 0.0f;
		MonData.WeaknessHP = 0.0f;
		MonData.Phase = EPhase::FirstPhase;
		CurHP = 0.0f;
		CurWeakHP = 0.0f;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventDispatcher_MonState, FCurMonState, Monster);
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
	struct FCurMonState CurMonState;

	UPROPERTY(EditAnywhere, Category = "Data", BlueprintReadWrite)
	EPhase Phase;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher", BlueprintCallable)
	FEventDispatcher_MonState EventDispatcherMonState;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher", BlueprintCallable)
	FEventDispatcher_UpdateHP EventDispatcherUpdateHP;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher", BlueprintCallable)
	FEventDispatcher_Death EventDispatcherDeath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* MonStateDataTable;

	UFUNCTION()
	void AddDamage(float Damage, FName BoneName, EPhase MonsterPhase);

	UFUNCTION()
	void SetMonState(FName RowName);

	// Only Debug?
	// void PrintHP(float MaxHP, float WeaknessHP, float CurHP, float CurWeakHP);

	UFUNCTION()
	bool IsDeath(FCurMonState State);
};
