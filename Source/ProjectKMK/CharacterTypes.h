#pragma once

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	EPS_Locomotion UMETA(DisplayName = "Locomotion"),
	EPS_Attack UMETA(DisplayName = "Attack"),
	EPS_Dodge UMETA(DisplayName = "Dodge"),
	EPS_HitReact UMETA(DisplayName = "HitReact"),
	EPS_Dead UMETA(DisplayName = "Dead"),
};

UENUM(BlueprintType)
enum class ETargetingMode : uint8
{
	ETM_NormalMode UMETA(DisplayName = "NormalMode"),
	ETM_TargetingMode UMETA(DisplayName = "TargetingMode"),
};