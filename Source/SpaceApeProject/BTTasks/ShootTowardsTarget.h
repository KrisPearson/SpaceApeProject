// // Copyright 2018 Kristian Pearson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ShootTowardsTarget.generated.h"

/**
 * 
 */
UCLASS()
class SPACEAPEPROJECT_API UShootTowardsTarget : public UBTTaskNode
{
	GENERATED_BODY()

		UShootTowardsTarget(const FObjectInitializer& objectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/* Used to handle the duration of the shooting, as well as the time between shooting alternatively*/
	//FTimerHandle ShotTimerHandle;

	float TimeSinceLastAction;

	float DelayUntilAction;
	
	bool bCanShoot = false;

	void ToggleShooting();

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
		struct FBlackboardKeySelector TargetActorKey;

};
