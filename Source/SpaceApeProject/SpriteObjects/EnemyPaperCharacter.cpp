// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyPaperCharacter.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Components/CapsuleComponent.h"
#include "Components/SpriteShadowComponent.h"
#include "Components/PaperCharacterAnimationComponent.h"
#include "BehaviorTree/BehaviorTree.h"
//#include "Controllers/BaseAiController.h"

#include "BehaviorTree/BehaviorTree.h"

AEnemyPaperCharacter::AEnemyPaperCharacter() {
	GetCapsuleComponent()->SetCollisionProfileName("Enemy");
}

void AEnemyPaperCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


}

bool AEnemyPaperCharacter::ReceiveDamage_Implementation(float DamageAmount, AActor * DamageInstigator, FGenericTeamId DamageFromTeam) {

	// TODO: Inform Controller of damage > Controller can then notify the Blackboard

	return Super::ReceiveDamage_Implementation(DamageAmount, DamageInstigator, DamageFromTeam);
}




void AEnemyPaperCharacter::BeginPlay() {
	Super::BeginPlay();


	Behaviour->BlackboardAsset;

	World = GetWorld();

	// Sets the Enemy Characters to team 2 // TODO: Predefine some team values
	SetGenericTeamId(FGenericTeamId(2));
}
