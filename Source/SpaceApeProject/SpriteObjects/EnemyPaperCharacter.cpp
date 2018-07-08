// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyPaperCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "PaperFlipbookComponent.h"
#include "Materials/MaterialInstance.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"

AEnemyPaperCharacter::AEnemyPaperCharacter() {
	GetCapsuleComponent()->SetCollisionProfileName("Enemy");
}

void AEnemyPaperCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyPaperCharacter, CurrentHealthPoints);
}

void AEnemyPaperCharacter::BeginPlay() {
	Super::BeginPlay();

	World = GetWorld();

	// Set the health points the class uses during play to the value of the enemy type's default health points.
	CurrentHealthPoints = HealthPoints;

	// Rotate the Sprite to face the negative X direction and tilt up to face the camera
	GetSprite()->AddLocalRotation(FRotator(00.f, 90.f, -30.f));

	UMaterialInstance* BaseMat = static_cast<UMaterialInstance*>(GetSprite()->GetMaterial(0));

	//Create a dynamic material in order to enable access to the damage flicker scaler parameter.
	if (BaseMat != nullptr) {
		DynamicEnemyMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
		GetSprite()->SetMaterial(0, DynamicEnemyMaterial);
	}
}


/*
This is called by the attacking class following a successful attack.
Returns a bool via reference in order to inform the attacking class of the enemy's demise.
*/
void AEnemyPaperCharacter::ReceiveDamage(int _DamageAmount, bool& _IsDead, int& _ScoreToAdd) {
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(" ReceiveDamage Called on Server =: %s"), Role == ROLE_Authority ? TEXT("True") : TEXT("False")));
	//DynamicEnemyMaterial->SetScalarParameterValue(FName("StartTime"), World->GetTimeSeconds());

	// could pass enemyscore value + damage here? Perhaps add to a single in return value, as opposed to pointer params? Leave it as is for now....

	if (Role < ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("WARNING: ServerReceiveDamage Disabled"));

		//	ServerReceiveDamage(_DamageAmount);
	}
	else {

	//	// Add the damage dealt to the score and check whether the damage dealt leaves the enemy at 0hp. If so, then remove the remainder.
	//	_ScoreToAdd = (CurrentHealthPoints - _DamageAmount) <= 0 ? (_DamageAmount - (CurrentHealthPoints % _DamageAmount)) : _DamageAmount;
	//	CurrentHealthPoints -= _DamageAmount;

	//	if (!CheckIfAlive())
	//	{
	//		_IsDead = true;
	//		EnemyDeath();

	//	}
	//	else {
	//		_IsDead = false;
			MulticastPlayDamageFlash();
	//	}
	}

}


/*
Tells the material to flicker, following the enemy recieving damage.
Called on server, and runs on server and all clients.
*/
void AEnemyPaperCharacter::MulticastPlayDamageFlash_Implementation() {
	if (Role == ROLE_Authority) {
		UE_LOG(LogTemp, Warning, TEXT(" MulticastPlayDamageFlash_Implementation Authority"));
	}
	else UE_LOG(LogTemp, Warning, TEXT(" MulticastPlayDamageFlash_Implementation Not Authority"));
	if (DynamicEnemyMaterial != nullptr) {
		if (World != nullptr) {
			// Send the current time to the material, which will then handle the duration of the flicker.
			// This approach means that the material's scaler value only needs to be set once, rather than updated by the AEnemy class each frame, or via Timer.
			DynamicEnemyMaterial->SetScalarParameterValue(FName("StartTime"), World->GetTimeSeconds());
		}
		else UE_LOG(LogTemp, Warning, TEXT(" Warning: World = Null"));
	}
	else  UE_LOG(LogTemp, Warning, TEXT(" Warning: DynamicEnemyMaterial = Null"));
}

/*
A check to see whether the enemy has health points remianing.
*/
bool AEnemyPaperCharacter::CheckIfAlive() {
	if (CurrentHealthPoints > 0) {
		return true;
	}
	else return false;
}

/*
Destroy this actor following a broadcast to the WaveManager.
*/
void AEnemyPaperCharacter::EnemyDeath() {
	//SpawnPickup(); 
	EnemyDeathDelegate.Broadcast(this);
	Destroy();
}