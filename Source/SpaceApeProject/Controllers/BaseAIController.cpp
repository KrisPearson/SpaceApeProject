// // Copyright 2018 Kristiam Pearson. All Rights Reserved.

#include "BaseAIController.h"
#include "Classes/GenericTeamAgentInterface.h"

ABaseAIController::ABaseAIController(const FObjectInitializer & ObjectInitializer) {


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

		ObjectInterface->GetGenericTeamId();

		FGenericTeamId TeamID;


		TeamID = GetGenericTeamId();
		UE_LOG(LogTemp, Warning, TEXT("teamID before = %d"), TeamID.GetId());
		TeamID = ObjectInterface->GetGenericTeamId();

		this->SetGenericTeamId(ObjectInterface->GetGenericTeamId());

		//TeamID = GetGenericTeamId();

		UE_LOG(LogTemp, Warning, TEXT("teamID after = %d"), TeamID.GetId());

		//SetGenericTeamId(IGenericTeamAgentInterface::Execute_GetGenericTeamId(GetPawn()));

		//IGenericTeamAgentInterface::Execute_GetGenericTeamId(GetPawn());

		//if (IDamageableInterface::Execute_RecieveDamage(OtherActor, (*WeaponData)->BaseWeaponDamage, (*WeaponData)->OwningTeam)) {

	}

	// Assign to Team 1
	//SetGenericTeamId(FGenericTeamId(1));
	//SetGenericTeamId(GetPawn()->GetGene); // TDO: Cast to Interface
}
