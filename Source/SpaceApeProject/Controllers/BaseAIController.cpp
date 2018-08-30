// // Copyright 2018 Kristiam Pearson. All Rights Reserved.

#include "BaseAIController.h"
#include "SpriteObjects/EnemyPaperCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "Classes/GenericTeamAgentInterface.h"


// https://www.youtube.com/watch?v=VxvahnKYB8E

ABaseAIController::ABaseAIController(const FObjectInitializer & ObjectInitializer) {

	//BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("Blackboard COmponent"));
	//BehaviourComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviourTree COmponent"));


	//Assign to Team 1
	//SetGenericTeamId(FGenericTeamId(5));
}



void ABaseAIController::BeginPlay() {
	Super::BeginPlay();
	//IGenericTeamAgentInterface::Execute_GetGenericTeamId();
	FindPawnTeamId();

}

/*Identifies the GenericTeamId of teh owned pawn and sets the controlelrs Id to match */
void ABaseAIController::FindPawnTeamId() {

	IGenericTeamAgentInterface* ObjectInterface = Cast<IGenericTeamAgentInterface>(GetPawn());
	if (ObjectInterface) {

		//ObjectInterface->GetGenericTeamId();

		FGenericTeamId TeamID;


		TeamID = GetGenericTeamId();
		UE_LOG(LogTemp, Warning, TEXT("teamID before = %d"), TeamID.GetId());
		TeamID = ObjectInterface->GetGenericTeamId();

		this->SetGenericTeamId(ObjectInterface->GetGenericTeamId());

		//TeamID = GetGenericTeamId();

		UE_LOG(LogTemp, Warning, TEXT("teamID after = %d"), TeamID.GetId());

		//SetGenericTeamId(IGenericTeamAgentInterface::Execute_GetGenericTeamId(GetPawn()));

		//IGenericTeamAgentInterface::Execute_GetGenericTeamId(GetPawn());

		//if (IDamageableInterface::Execute_ReceiveDamage(OtherActor, (*WeaponData)->BaseWeaponDamage, (*WeaponData)->OwningTeam)) {

	}

	// Assign to Team 1
	//SetGenericTeamId(FGenericTeamId(1));
	//SetGenericTeamId(GetPawn()->GetGene); // TDO: Cast to Interface
}

void ABaseAIController::Possess(APawn * InPawn) {
	Super::Possess(InPawn);

	AEnemyPaperCharacter* Enemy = Cast<AEnemyPaperCharacter>(InPawn);
	if (Enemy && Enemy->Behaviour) {
		//BlackboardComp->InitializeBlackboard( *(Enemy->Behaviour->BlackboardAsset) );
	}
}
