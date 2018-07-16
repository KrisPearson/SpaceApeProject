// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
//#include "UObject/NoExportTypes.h"
//#include "UObject.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "UObject/Interface.h"
#include "SpriteCharacterInterface.generated.h"



UINTERFACE(Blueprintable)
class USpriteCharacterInterface : public UInterface {
	GENERATED_UINTERFACE_BODY()
};

class SPACEAPEPROJECT_API ISpriteCharacterInterface {
	GENERATED_IINTERFACE_BODY()

public:
	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Player interaction")
	//void Interact();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Sprite")
	FVector GetCharacterFaceDirection() const ;
};