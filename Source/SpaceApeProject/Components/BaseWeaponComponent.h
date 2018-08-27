// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Components/ActorComponent.h"
#include "SpriteObjects/BaseProjectile.h"
#include "Structs/WeaponData.h"
#include "BaseWeaponComponent.generated.h"

enum class EWeaponTier : uint8;

class UObjectPoolComponent;

UCLASS(Blueprintable)
class SPACEAPEPROJECT_API UBaseWeaponComponent : public UActorComponent {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ProjectileParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class USoundBase* FireAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class USoundBase* HitAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true", ClampMin = "0.1", ClampMax = "10"))
	float DelayBetweenShots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true", ClampMin = "10", ClampMax = "500"))
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess = "true", ClampMin = "100", ClampMax = "8000"))
	float Speed;

	//TSubclassOf<class ABaseProjectile> ProjectileToSpawn = ABaseProjectile::StaticClass(); // We may wish to spawn different projectile types (grenads etc)
	//class ABaseProjectile ProjectileToSpawn;

	UBaseWeaponComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	FWeaponData WeaponData;

	class ABasePaperCharacter* OwningCharacter;

	UWorld* World;

	// A reference to the character's projectile object pool
	UObjectPoolComponent* PlayerProjectilePoolRef;

	void CheckAndUpdateProjectile(class ABaseProjectile* _Projectile);

public:

	void SetObjectPoolReference(class UObjectPoolComponent* _PoolRef) { PlayerProjectilePoolRef = _PoolRef; }

	UObjectPoolComponent* GetObjectPoolReference();

	virtual void Shoot(FVector _FireDirection);



	float GetFireRate() { return WeaponData.WeaponFireRate; }

	USoundBase* GetFireSound() { return WeaponData.FireSound; }

	FWeaponData* GetWeaponData() { return &WeaponData; } // TODO: Should WeaponData be private or const?

private:

	/* Timer for controlling fire rate */
	FTimerHandle TimerHandle_ShotTimerExpired;

	/* Sets the weapon as ReadyToFire. Called by the TimerHandle*/
	void ShotTimerExpired();

	bool bReadyToFire = true;
};


