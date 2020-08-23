// // Copyright 2018 Kristian Pearson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ChooseRandomLocationInBounds.generated.h"

/**
 * 
 */
UCLASS()
class SPACEAPEPROJECT_API UChooseRandomLocationInBounds : public UBTTaskNode
{
	GENERATED_BODY()
	
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
		struct FBlackboardKeySelector TargetLocationKey;
	
};
