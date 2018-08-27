// // Copyright 2018 Kristiam Pearson. All Rights Reserved.

#include "ChooseRandomLocationInBounds.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SpriteObjects//BasePaperCharacter.h"
#include "AIController.h"

EBTNodeResult::Type UChooseRandomLocationInBounds::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) {

	auto Controller = OwnerComp.GetAIOwner(); // ->GetBlackboardComponent()->SetValueAsVector()
	auto ControlledPawn = Controller->GetPawn();
	auto Character = Cast<ABasePaperCharacter>(ControlledPawn);

	FVector MaxBounds =Character->GetRoomMaxBounds();
	FVector MinBounds = Character->GetRoomMinBounds();

	auto BlackboardComp = OwnerComp.GetBlackboardComponent();
	auto TargetLocation = BlackboardComp->GetValueAsVector(TargetLocationKey.SelectedKeyName);


	FVector Location = FVector(
		FMath::FRandRange(MinBounds.X, MaxBounds.X),
		FMath::FRandRange(MinBounds.Y, MaxBounds.Y),
		0
	);

	BlackboardComp->SetValueAsVector(TargetLocationKey.SelectedKeyName, Location);

	return EBTNodeResult::Succeeded;
}
