// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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


	/** Function to handle the projectile hitting something */
	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


#pragma region ProjectileBaseClass
	//virtual void ExecuteHitEvent();
#pragma endregion



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnEnemyHit OnEnemyHit;


private:

	UWorld* World;

	//inline void BroadcastHit(AActor* _HitActor, int _DamageAmount) {
	//	if (OnEnemyHit.IsBound())  OnEnemyHit.Broadcast(_HitActor, (*WeaponData)->BaseWeaponDamage);
	//}

	inline void BroadcastHit(AActor* _HitActor, int _DamageAmount) {
		UE_LOG(LogTemp, Warning, TEXT("Broadcast hit"));
		if (OnEnemyHit.IsBound()) {
			OnEnemyHit.Broadcast(_HitActor, _DamageAmount);
			UE_LOG(LogTemp, Warning, TEXT("Broadcast hit: Bound = true"));
		}
	}

	
};
