// // Copyright 2018 Kristian Pearson. All Rights Reserved.
#include "ShootTowardsTarget.h"
#include "SpriteObjects/BasePaperCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UShootTowardsTarget::UShootTowardsTarget(const FObjectInitializer& objectInitializer)  {
	bNotifyTick = true;
}

EBTNodeResult::Type UShootTowardsTarget::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) {
	return EBTNodeResult::InProgress;

}

void UShootTowardsTarget::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds) {
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	TimeSinceLastAction += DeltaSeconds;

	if (TimeSinceLastAction > DelayUntilAction) {

		auto Controller = OwnerComp.GetAIOwner();
		auto ControlledPawn = Controller->GetPawn();
		auto Character = Cast<ABasePaperCharacter>(ControlledPawn);

		if (bCanShoot) { // Start Shooting

			auto BlackboardComp = OwnerComp.GetBlackboardComponent();
			auto* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));

			if (TargetActor != nullptr) {

				FVector Direction = TargetActor->GetActorLocation() - Character->GetActorLocation();
				Direction.Normalize();

				Character->SetShootAxisValues(FMath::RoundToInt(Direction.Y), FMath::RoundToInt(Direction.X));

			}
		}
		else { // Stop Shooting

			Character->SetShootAxisValues(0, 0);

		}

		ToggleShooting();

	}

}

EBTNodeResult::Type UShootTowardsTarget::AbortTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) {
	Super::AbortTask(OwnerComp, NodeMemory);

	//UE_LOG(LogTemp, Warning, TEXT(" UShootTowardsTarget::AbortTask "));

	auto Controller = OwnerComp.GetAIOwner();
	auto ControlledPawn = Controller->GetPawn();
	auto Character = Cast<ABasePaperCharacter>(ControlledPawn);

	// Stop Shooting
	if (Character) Character->SetShootAxisValues(0, 0);

	return EBTNodeResult::Succeeded;
}

void UShootTowardsTarget::ToggleShooting() {
	bCanShoot = !bCanShoot;

	DelayUntilAction = FMath::FRandRange(1, 5); // TODO: Add "Aggressiveness" parameter to influence the time between attacks.


	TimeSinceLastAction = 0;
}

