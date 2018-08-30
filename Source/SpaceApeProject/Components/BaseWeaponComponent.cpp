// Fill out your copyright notice in the Description page of Project Settings.

/**
BaseWeaponComponent.cpp
Purpose: Defines the properties of a weapon including stats and artwork to be applied to projectiles.

The BaseWeaponComponent is intended to be inherrited by Weapons in order to create different varients of weapons.

@author Kristian Pearson
@version 1.0 17/18/2018
*/

#include "Components/BaseWeaponComponent.h"
#include "Components/ObjectPoolComponent.h"
#include "Particles/ParticleSystem.h"
#include "UObject/ConstructorHelpers.h"
#include "Interfaces/SpriteObjectInterface.h"
#include "SpriteObjects/BasePaperCharacter.h"

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
	OwningCharacter = Cast<ABasePaperCharacter>(GetOwner());

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
	}

	UE_LOG(LogTemp, Warning, TEXT("WeaponData. Delay = : %f"), DelayBetweenShots);
}

// Called every frame
void UBaseWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

/*
This overridable method gets projectiles from the character's object pool component and "spawns" them.
Should be called by the server by an RPC when networking.
*/
void UBaseWeaponComponent::Shoot(FVector _FireDirection) {

	if (bReadyToFire) {

		const FRotator FireRotation = _FireDirection.Rotation();

		FVector GunOffset = FVector(0.f, 0.f, -30.f);

		const FVector SpawnLocation = GetOwner()->GetActorLocation() + FireRotation.RotateVector(GunOffset);

		ABaseProjectile* Projectile = Cast<ABaseProjectile>(PlayerProjectilePoolRef->GetReusableReference());

		FProjectileLaunchData LaunchData(SpawnLocation, _FireDirection, OwningCharacter, OwningCharacter->GetGenericTeamId());

		if (Projectile != nullptr) {
			CheckAndUpdateProjectile(Projectile);
			Projectile->LaunchProjectile(LaunchData);

			//Projectile->SetProjectileLocationAndDirection(SpawnLocation, _FireDirection, true);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("UBaseWeaponComponent::Shoot() Projectile is nullptr."));
		}

		bReadyToFire = false;
		if (UWorld* World = GetWorld()) {
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &UBaseWeaponComponent::ShotTimerExpired, WeaponData.WeaponFireRate);
		}
	}
}

void UBaseWeaponComponent::ShotTimerExpired() {
	bReadyToFire = true;
}

UObjectPoolComponent * UBaseWeaponComponent::GetObjectPoolReference() {
	if (PlayerProjectilePoolRef != nullptr) {
		return PlayerProjectilePoolRef;
	}
	else return nullptr;
}

/*Compares the ID of the projectile to that of this weapon component. If they do not match, 
then the projectile is updated to match the WeaponData of this weapon. */
void UBaseWeaponComponent::CheckAndUpdateProjectile(ABaseProjectile * _Projectile) {
	if (_Projectile->GetWeaponDataID() != GetUniqueID()) {
		// update the weapon data of the projectile, and update the id
		_Projectile->PassNewWeaponData(WeaponData, GetUniqueID());
	}
}

