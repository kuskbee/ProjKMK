// Fill out your copyright notice in the Description page of Project Settings.


#include "KMKGameModeBase.h"
#include "KMKPlayerController.h"
#include "PlayerCharacter.h"

AKMKGameModeBase::AKMKGameModeBase()
{
	PlayerControllerClass = AKMKPlayerController::StaticClass();
	DefaultPawnClass = APlayerCharacter::StaticClass();
}
