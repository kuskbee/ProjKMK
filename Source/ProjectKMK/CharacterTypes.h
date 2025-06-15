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