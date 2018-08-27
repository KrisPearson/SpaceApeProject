// // Copyright 2018 Kristiam Pearson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseAIController.generated.h"

/**
 * 
 */
UCLASS()
class SPACEAPEPROJECT_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

		ABaseAIController(const FObjectInitializer& ObjectInitializer);
	
	void BeginPlay() override;


	void FindPawnTeamId();

};
