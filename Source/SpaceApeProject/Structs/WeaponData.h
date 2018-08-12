#pragma once


#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Components/ActorComponent.h"
#include "Enums/TeamOwnerEnum.h"

#include "WeaponData.generated.h"


class UProjectileMovementComponent;
class UBaseProjectileComponent;
class UStaticMeshComponent;
class UParticleSystem;
class UStaticMesh;

/*
THis struct needs to be initialised by each weapon's constructor.
The information here will be used to change weapon effects as well as projectile art.
*/

USTRUCT(BlueprintType)
struct FWeaponData {
	GENERATED_BODY()

		//FWeaponData() {};
	FWeaponData(UParticleSystem* _ProjectileParticleSystem = nullptr, UParticleSystem* _HitEffectParticleSystem = nullptr, USoundBase* _FireSound = nullptr, USoundBase* _HitSound = nullptr, float _WeaponFireRate = 1, int _BaseWeaponDamage = 10, float _BaseProjectileSpeed = 1000, TeamOwner::ETeamOwner _OwningTeam = TeamOwner::ETeamOwner::TO_NoOwner) {
		ProjectileParticleSystem = _ProjectileParticleSystem;
		HitEffectParticleSystem = _HitEffectParticleSystem;
		FireSound = _FireSound;
		HitSound = _HitSound;
		WeaponFireRate = _WeaponFireRate;
		BaseWeaponDamage = _BaseWeaponDamage;
		BaseProjectileSpeed = _BaseProjectileSpeed;
		OwningTeam = _OwningTeam;
	};

	//Projectile Art
	//Necessary components(an array, perhaps ? )

	// type data used to add new components to projectiles. (needed here?)
	//class BaseProjectileComponent ProjectileComponent;

	//UPROPERTY(BlueprintReadWrite, Category = "WeaponData")
	TArray< TSubclassOf<UBaseProjectileComponent> > ProjectileComponents;

	// The mesh used on the projectile
	UPROPERTY(BlueprintReadWrite, Category = "WeaponData")
		UStaticMesh* ProjectileMeshComponent;

	// The Particle System of the projectile
	UPROPERTY(BlueprintReadWrite, Category = "WeaponData")
		UParticleSystem* ProjectileParticleSystem;

	UPROPERTY(BlueprintReadWrite, Category = "WeaponData")
		UParticleSystem* HitEffectParticleSystem;

	UPROPERTY(BlueprintReadWrite, Category = "WeaponData")
		USoundBase* FireSound; // The audio effect played when firing a projectile

	UPROPERTY(BlueprintReadWrite, Category = "WeaponData")
		USoundBase* HitSound;

	// The delay between shots
	UPROPERTY(BlueprintReadWrite, Category = "WeaponData")
		float WeaponFireRate;

	// The Damage dealt by the projectiles
	UPROPERTY(BlueprintReadWrite, Category = "WeaponData")
		int BaseWeaponDamage;

	// The base movement speed of the projectile
	UPROPERTY(BlueprintReadWrite, Category = "WeaponData")
		float BaseProjectileSpeed;

	TeamOwner::ETeamOwner OwningTeam;

	//UPROPERTY(BlueprintReadWrite, Category = "WeaponData")
	//	TEnumAsByte<TeamOwner::ETeamOwner> OwningTeam;


};
