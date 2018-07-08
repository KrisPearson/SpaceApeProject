// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterPlayerController.h"


ACharacterPlayerController::ACharacterPlayerController() {
	UE_LOG(LogTemp, Warning, TEXT("ACharacterPlayerController Constructor"));
	
}

void ACharacterPlayerController::Possess(APawn * aPawn) {
	Super::Possess(aPawn);

	UE_LOG(LogTemp, Warning, TEXT("ACharacterPlayerController Possess"));
}

