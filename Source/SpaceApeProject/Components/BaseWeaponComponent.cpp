// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BaseWeaponComponent.h"
#include "Components/ObjectPoolComponent.h"
#include "Particles/ParticleSystem.h"
#include "UObject/ConstructorHelpers.h"
#include "SpriteObjects/PlayerPaperCharacter.h"

/*
The Player Weapon Component dictates:
> the Projectile type
> The WeaponData preset
> The Spawn pattern (?)
*/


// Sets default values for this component's properties
UBaseWeaponComponent::UBaseWeaponComponent() {

	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UBaseWeaponComponent::BeginPlay() {
	Super::BeginPlay();

	World = GetWorld();
	OwningCharacter = Cast<APlayerPaperCharacter>(GetOwner());

	if (OwningCharacter) {
		WeaponData = FWeaponData(
			ProjectileParticle,
			HitParticle,
			FireAudio,
			HitAudio,
			DelayBetweenShots,
			Damage,
			Speed
		);
	};
}

// Called every frame
void UBaseWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

/*
This overridable method gets projectiles from the character's object pool component and "spawns" them.
should be called by the server by an RPC when networking.
*/
void UBaseWeaponComponent::Shoot(FVector _FireDirection) {

	const FRotator FireRotation = _FireDirection.Rotation();

	FVector GunOffset = FVector(0.f, 0.f, -30.f);

	const FVector SpawnLocation = GetOwner()->GetActorLocation() + FireRotation.RotateVector(GunOffset);

	ABaseProjectile* Projectile = Cast<ABaseProjectile>(PlayerProjectilePoolRef->GetReusableReference());

	if (Projectile != nullptr) {
		CheckAndUpdateProjectile(Projectile);
		Projectile->SetProjectileLocationAndDirection(SpawnLocation, _FireDirection, true);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("UBaseWeaponComponent::Shoot() Projectile is nullptr."));
	}

}

UObjectPoolComponent * UBaseWeaponComponent::GetObjectPoolReference() {
	if (PlayerProjectilePoolRef != nullptr) {
		return PlayerProjectilePoolRef;
	}
	else return nullptr;
}

void UBaseWeaponComponent::CheckAndUpdateProjectile(ABaseProjectile * _Projectile) {
	if (_Projectile->GetWeaponDataID() != GetUniqueID()) {
		// update the weapon data of the projectile, and update the id
		_Projectile->PassNewWeaponData(WeaponData, GetUniqueID());
		UE_LOG(LogTemp, Warning, TEXT(" PassNewWeaponData %f"),  WeaponData.BaseWeaponDamage);

	}
}

