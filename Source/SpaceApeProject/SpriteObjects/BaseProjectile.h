// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/WeaponData.h"
#include "BaseProjectile.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyHit, AActor*, _Enemy, int, _Amount);

UCLASS()
class SPACEAPEPROJECT_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseProjectile();


protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		class UProjectileMovementComponent* ProjectileMovement;

	/** Mesh collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* ProjectileMesh;

	/** Projectile Particle System**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"), Replicated)
		class UParticleSystemComponent* ProjectileParticle;

	/** Projectile Particle System - Played on colission**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"), Replicated)
		class UParticleSystemComponent* HitEffectParticle;

	/** Sound played on colission */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
		class USoundBase* HitSoundEffect;


	/** Function to handle the projectile hitting something */
	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Replicated)
		float ProjectileDamage = 10.f;

	// The move speed of the projectile. This is used in SetVelocityDirection to set the velocity 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Replicated)
		float CurrentMoveSpeed = 1000;


	UFUNCTION(NetMulticast, Reliable)
		void MulticastAssignWeaponDataValues(UParticleSystem* _NewParticleSystem, UParticleSystem* _NewHitParticleSystem, USoundBase* _HitSound, float _NewSpeed);
	void MulticastAssignWeaponDataValues_Implementation(UParticleSystem* _NewParticleSystem, UParticleSystem* _NewHitParticleSystem, USoundBase* _HitSound, float _NewSpeed);


public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	void ToggleEnabled(bool _value);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnEnemyHit OnEnemyHit;

	void SetPoolReference(class UObjectPoolComponent* _PoolRef) { OwningPool = _PoolRef; }

	void SetProjectileLocationAndDirection(FVector _Loc, FVector _Vel, bool _ToggleEnabled);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastSetLocationAndVelocityDirection(FVector _Loc, FVector _Vel, bool _ToggleEnabled);
	void MulticastSetLocationAndVelocityDirection_Implementation(FVector _Loc, FVector _Vel, bool _ToggleEnabled);

	void ResetProjectile();

	inline void SetWeaponData(FWeaponData** _WeaponData) { WeaponData = _WeaponData; };

	void PassNewWeaponData(struct FWeaponData _NewWeaponData, int _NewWeaponDataID);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastAssignNewWeaponData(FWeaponData _NewWeaponData);
	void MulticastAssignNewWeaponData_Implementation(FWeaponData _NewWeaponData);

	int GetWeaponDataID() { return WeaponDataID; }


private:

	class UObjectPoolComponent* OwningPool;

	FTimerHandle ReturnToPoolTimer;

	UPROPERTY(Replicated)
		int WeaponDataID;

	UWorld* World;

	// Points towards the WeaponData pointer contained in the owning PlayerCharacter object.
	FWeaponData** WeaponData;

	inline void BroadcastHit(AActor* _HitActor, int _DamageAmount) {
		UE_LOG(LogTemp, Warning, TEXT("Broadcast hit"));
		if (OnEnemyHit.IsBound()) {
			OnEnemyHit.Broadcast(_HitActor, _DamageAmount);
			UE_LOG(LogTemp, Warning, TEXT("Broadcast hit: Bound = true"));
		}
	}

	
};
