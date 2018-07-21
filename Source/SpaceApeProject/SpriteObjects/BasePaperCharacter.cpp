// // Copyright 2018 Kristiam Pearson. All Rights Reserved.

#include "BasePaperCharacter.h"
#include "Net/UnrealNetwork.h"

#include "Components/BaseWeaponComponent.h"
#include "Components/ObjectPoolComponent.h"
#include "Components/PaperCharacterAnimationComponent.h"
#include "Components/SpriteShadowComponent.h"

#include "SpriteObjects/BaseProjectile.h"

void ABasePaperCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(APlayerPaperCharacter, bIsShooting);
	DOREPLIFETIME(ABasePaperCharacter, CurrentMovingDirection);
	DOREPLIFETIME(ABasePaperCharacter, CurrentShootingDirection);
	DOREPLIFETIME(ABasePaperCharacter, CurrentHealthPoints);
}





ABasePaperCharacter::ABasePaperCharacter() {

	ShadowComponent = CreateDefaultSubobject<USpriteShadowComponent>(TEXT("ShadowComponent"));

	AnimationComponent = CreateDefaultSubobject<UPaperCharacterAnimationComponent>(TEXT("AnimationComponent"));

	ProjectilePool = CreateDefaultSubobject<UObjectPoolComponent>(TEXT("ProjectilePool"));
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


#pragma region CharacterInterface Methods 

void ABasePaperCharacter::DealDamage(AActor * ActorToDamage, int DamageAmount) {

	ISpriteCharacterInterface* CharacterInterface = Cast<ISpriteCharacterInterface>(ActorToDamage);
	 
	if (CharacterInterface) {


		ISpriteCharacterInterface::Execute_RecieveDamage(ActorToDamage, DamageAmount);

		//CharacterInterface->Execute_RecieveDamage(_DamageAmount);

	}
}

FVector ABasePaperCharacter::GetCharacterFaceDirection_Implementation() const {
	return FaceDirectionVector;
}

bool ABasePaperCharacter::RecieveDamage_Implementation(int DamageAmount) {

	UE_LOG(LogTemp, Warning, TEXT("ABasePaperCharacter::RecieveDamage_Implementation"));


	CurrentHealthPoints -= DamageAmount;

	return true;
}

//FVector ABasePaperCharacter::GetCharacterFaceDirection_Implementation() const {
//	check(0 && "GetCharacterFaceDirection_Implementation method requires override  method")
//	return FVector();
//}

//
//bool ABasePaperCharacter::DealDamage_Implementation(ABasePaperCharacter * CharacterToDamage, int _DamageAmount) {
//
//	return true;
//}

#pragma endregion
