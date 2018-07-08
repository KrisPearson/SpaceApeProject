// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CharacterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SPACEAPEPROJECT_API ACharacterPlayerController : public APlayerController
{
	GENERATED_BODY()


		ACharacterPlayerController();
	

	virtual void Possess(APawn* aPawn) override;
};
