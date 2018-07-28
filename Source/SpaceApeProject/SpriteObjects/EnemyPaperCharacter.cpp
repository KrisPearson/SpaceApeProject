// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyPaperCharacter.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Components/CapsuleComponent.h"
#include "Components/SpriteShadowComponent.h"
#include "Components/PaperCharacterAnimationComponent.h"

#include "BehaviorTree/BehaviorTree.h"

AEnemyPaperCharacter::AEnemyPaperCharacter() {
	GetCapsuleComponent()->SetCollisionProfileName("Enemy");

	//ShadowComponent = CreateDefaultSubobject<USpriteShadowComponent>(TEXT("ShadowComponent"));

	//AnimationComponent = CreateDefaultSubobject<UPaperCharacterAnimationComponent>(TEXT("AnimationComponent"));

}

void AEnemyPaperCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


}

void AEnemyPaperCharacter::BeginPlay() {
	Super::BeginPlay();

	World = GetWorld();


}
