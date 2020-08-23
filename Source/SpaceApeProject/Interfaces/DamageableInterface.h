// // Copyright 2018 Kristian Pearson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Enums/TeamOwnerEnum.h"
#include "Classes/GenericTeamAgentInterface.h"
#include "DamageableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageableInterface : public UInterface {
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class SPACEAPEPROJECT_API IDamageableInterface {
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
		bool ReceiveDamage(float DamageAmount, AActor* DamageInstigator, FGenericTeamId DamageFromTeam);
	
};
