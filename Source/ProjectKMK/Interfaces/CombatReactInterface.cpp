// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatReactInterface.h"

// Add default functionality here for any ICombatReactInterface functions that are not pure virtual.

bool ICombatReactInterface::ApplyHit(const FHitResult& HitResult, AActor* HitterActor)
{
	return false;
}
