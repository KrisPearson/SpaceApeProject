// // Copyright 2018 Kristiam Pearson. All Rights Reserved.

#include "BasePaperCharacter.h"
#include "Net/UnrealNetwork.h"

#include "Components/BaseWeaponComponent.h"
#include "Components/ObjectPoolComponent.h"
#include "Components/PaperCharacterAnimationComponent.h"
#include "Components/SpriteShadowComponent.h"

#include "SpriteObjects/BaseProjectile.h"

#include "Kismet/GameplayStatics.h"



ABasePaperCharacter::ABasePaperCharacter() {

	ShadowComponent = CreateDefaultSubobject<USpriteShadowComponent>(TEXT("ShadowComponent"));

	AnimationComponent = CreateDefaultSubobject<UPaperCharacterAnimationComponent>(TEXT("AnimationComponent"));

	ProjectilePool = CreateDefaultSubobject<UObjectPoolComponent>(TEXT("ProjectilePool"));

	//if (HasAuthority()) {
	//	this->SetReplicates(true);
	//}

	bReplicates = true;
}


void ABasePaperCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasePaperCharacter, bIsShooting);
	//DOREPLIFETIME(ABasePaperCharacter, CurrentMovingDirection);
	DOREPLIFETIME(ABasePaperCharacter, CurrentShootingDirection);
	DOREPLIFETIME(ABasePaperCharacter, CurrentHealthPoints);

	UE_LOG(LogTemp, Warning, TEXT("ABasePaperCharacter::GetLifetimeReplicatedProps"));
}

void ABasePaperCharacter::BeginPlay() {
	Super::BeginPlay();

	World = GetWorld();

	ChangeWeapon(DefaultWeaponComponent);

	if (Role == ROLE_Authority) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(" FillPool Called on Server =: %s"), Role == ROLE_Authority ? TEXT("True") : TEXT("False")));
		ProjectilePool->FillPool(ABaseProjectile::StaticClass(), DefaultProjectilePoolSize);
		if (EquippedWeaponComponent) {
			EquippedWeaponComponent->SetObjectPoolReference(ProjectilePool);
			EquippedWeaponData = EquippedWeaponComponent->GetWeaponData();
		}

		TArray<AActor*>* PoolArray = ProjectilePool->GetArrayPointer();
		for (AActor* Actor : *PoolArray) {

			Cast<ABaseProjectile>(Actor)->SetWeaponData(&EquippedWeaponData);
			Cast<ABaseProjectile>(Actor)->OnEnemyHit.AddDynamic(this, &ABasePaperCharacter::DealDamage);
			Cast<ABaseProjectile>(Actor)->SetPoolReference(ProjectilePool);
		}
	}
}

void ABasePaperCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	UpdateFaceDirection();
	
	UpdateIsShooting();


	//if (Role == ROLE_Authority) {
	//	GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::Blue, FString::Printf(TEXT(" ROLE_Authority")));
	//	GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::White, FString::Printf(TEXT(" bIsShooting =: %s "), bIsShooting ? TEXT("True") : TEXT("False")));
	//	//GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::White, FString::Printf(TEXT(" ReplicatedIsShooting =: %s "), ReplicatedIsShooting ? TEXT("True") : TEXT("False")));
	//}
	//else if (Role == ROLE_AutonomousProxy) {
	//	GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::Blue, FString::Printf(TEXT(" ROLE_AutonomousProxy")));
	//	GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::White, FString::Printf(TEXT(" bIsShooting =: %s "), bIsShooting ? TEXT("True") : TEXT("False")));
	//	//GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::White, FString::Printf(TEXT(" ReplicatedIsShooting =: %s "), ReplicatedIsShooting ? TEXT("True") : TEXT("False")));
	//}
	//else if (Role == ROLE_SimulatedProxy) {
	//	GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::Blue, FString::Printf(TEXT(" ROLE_SimulatedProxy")));
	//	GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::White, FString::Printf(TEXT(" bIsShooting =: %s "), bIsShooting ? TEXT("True") : TEXT("False")));
	//	//GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::White, FString::Printf(TEXT(" ReplicatedIsShooting =: %s "), ReplicatedIsShooting ? TEXT("True") : TEXT("False")));
	//}

}
//
//void ABasePaperCharacter::SetIsShooting(bool newValue) {
//
//	bIsShooting = newValue;
//
//	if (bIsShooting) {
//		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, FString::Printf(TEXT(" ROLE_Authority =: %s for %s"), Role == ROLE_Authority ? TEXT("True") : TEXT("False"), *GetFName().ToString()));
//		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, FString::Printf(TEXT(" ROLE_AutonomousProxy =: %s for %s"), Role == ROLE_AutonomousProxy ? TEXT("True") : TEXT("False"), *GetFName().ToString()));
//		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, FString::Printf(TEXT(" ROLE_SimulatedProxy =: %s for %s"), Role == ROLE_SimulatedProxy ? TEXT("True") : TEXT("False"), *GetFName().ToString()));
//	}
//}



/*
This will be used primarily for weapon pickups.
Replaces the current weapon witha  new one.
Need to destroy old component and perhaps perform some kind of check.
This method should eventually be made private/ protected, and some kind of public condition check method should handle weapon changes.
*/
void ABasePaperCharacter::ChangeWeapon(TSubclassOf<UBaseWeaponComponent> _NewWeapon) {

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(" ChangeWeapon. Server =: %s"), Role == ROLE_Authority ? TEXT("True") : TEXT("False")));

	if (_NewWeapon) {

		//EquippedWeaponComponent = ConstructObject<UUBaseWeaponComponent>(_NewWeapon, this, *_NewWeapon->GetName()/*TEXT("InitialWeapon")*/);
		if (EquippedWeaponComponent != nullptr) EquippedWeaponComponent->DestroyComponent();
		EquippedWeaponComponent = NewObject<UBaseWeaponComponent>(this, _NewWeapon, *_NewWeapon->GetName());
		EquippedWeaponComponent->SetObjectPoolReference(ProjectilePool);

		EquippedWeaponComponent->RegisterComponent(); // this has been added to enable the component tick for some components
		EquippedWeaponData = EquippedWeaponComponent->GetWeaponData();
		FireSound = EquippedWeaponComponent->GetFireSound();



		//FWeaponData NewWeaponData = EquippedWeaponComponent->GetWeaponData();
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Emerald, FString::Printf(TEXT(" GetWeaponData Speed =  %f. Is Server = %s"), NewWeaponData.BaseProjectileSpeed, Role == ROLE_Authority ? TEXT("True") : TEXT("False")));
	}

}

void ABasePaperCharacter::UpdateFaceDirection() {

	FVector CurrentVelocity = GetVelocity();
	CurrentVelocity.Normalize();
	//UE_LOG(LogTemp, Warning, TEXT("Player Velocity  =  %f , %f, %f"), GetVelocity().X, GetVelocity().Y, GetVelocity().Z);
	//UE_LOG(LogTemp, Warning, TEXT("CurrentVelocity  =  %f , %f, %f"), CurrentVelocity.X, CurrentVelocity.Y, CurrentVelocity.Z);

	if (CurrentVelocity.SizeSquared() == 0) {
		// use last face direction
		if (bIsShooting) FaceDirectionVector = CurrentShootingDirection;
		return;
	}
	else if (FMath::Abs(CurrentVelocity.X) > FMath::Abs(CurrentVelocity.Y)) {
		// Vertical axis
		if (CurrentVelocity.X > 0) {
			//CurrentMovingDirection = EFaceDirection::FD_Up;
			FaceDirectionVector = SpriteDirection::Up;
		}
		else {
			//CurrentMovingDirection = EFaceDirection::FD_Down;
			FaceDirectionVector = SpriteDirection::Down;
		}
	}
	else {
		//Horizontal axis
		if (CurrentVelocity.Y > 0) {
			//CurrentMovingDirection = EFaceDirection::FD_Right;
			FaceDirectionVector = SpriteDirection::Right;
		}
		else {
			//CurrentMovingDirection = EFaceDirection::FD_Left;
			FaceDirectionVector = SpriteDirection::Left;
		}
	}
}

/* Establishes whether the character is shooting and, if so, replicates the bIsShooting property across to all players */
void ABasePaperCharacter::UpdateIsShooting() {
	if (IsLocallyControlled()) { // Only update if the character is owned by this player
		const bool newBIsShooting = CurrentHorizontalShootValue != 0 || CurrentVerticalShootValue != 0;
		if (newBIsShooting != bIsShooting) {
			bIsShooting = newBIsShooting; // Local update to prevent latency causing firing after input is realeased
			if (HasAuthority()) MulticastSetIsShooting(newBIsShooting); // Server player
			else if (Role == ROLE_AutonomousProxy) ServerSetIsShooting(newBIsShooting); // Client Player		
		}
	}
}



#pragma region CharacterInterface Methods 

void ABasePaperCharacter::DealDamage(AActor * ActorToDamage, int DamageAmount) {

	ISpriteObjectInterface* ObjectInterface = Cast<ISpriteObjectInterface>(ActorToDamage);
	 
	if (ObjectInterface) {

		ISpriteObjectInterface::Execute_RecieveDamage(ActorToDamage, DamageAmount);

	}
}

FVector ABasePaperCharacter::GetObjectFaceDirection_Implementation() const {

	return FaceDirectionVector;
}

bool ABasePaperCharacter::RecieveDamage_Implementation(int DamageAmount) {

	UE_LOG(LogTemp, Warning, TEXT("ABasePaperCharacter::RecieveDamage_Implementation"));


	CurrentHealthPoints -= DamageAmount;

	return true;
}

//FVector ABasePaperCharacter::GetObjectFaceDirection_Implementation() const {
//	check(0 && "GetObjectFaceDirection_Implementation method requires override  method")
//	return FVector();
//}

//
//bool ABasePaperCharacter::DealDamage_Implementation(ABasePaperCharacter * CharacterToDamage, int _DamageAmount) {
//
//	return true;
//}

#pragma endregion
