// Copyright 1998-2017 Epic Games, Inc. All Rights Reserve

#include "SpriteObjects/BaseProjectile.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"
#include "ParticleDefinitions.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/ObjectPoolComponent.h"
#include "Components/SphereComponent.h" 
#include "Sound/SoundBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "SpriteObjects/EnemyPaperCharacter.h"

#include "Engine.h"



/*
The Projectile is responsible for:
> Handling Colission
> The Speed of the projectile
> Projectile art (meshes, particles)
*/


void ABaseProjectile::BeginPlay() {
	Super::BeginPlay();

	World = GetWorld();
}

ABaseProjectile::ABaseProjectile() {

	ProjectileCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollissionMesh"));
	ProjectileCollisionSphere->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileCollisionSphere->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnHit);
	ProjectileCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnComponentEnterTrigger);
	ProjectileCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseProjectile::OnComponentExitTrigger);

	RootComponent = ProjectileCollisionSphere;

	ProjectileParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileParticle"));
	ProjectileParticle->SetupAttachment(RootComponent);

	HitEffectParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HitParticle"));
	HitEffectParticle->SetupAttachment(RootComponent);
	HitEffectParticle->DeactivateSystem();

	ProjectileParticle->EmitterDelay = 0.0f;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = ProjectileCollisionSphere;
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 15000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->Bounciness = 1.0f; // Does not lose velocity when bounces.
	ProjectileMovement->SetUpdatedComponent(GetRootComponent()); 

	bReplicates = true;

	ToggleEnabled(false);

	CurrentMoveSpeed = 1500;

}

void ABaseProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseProjectile, CurrentMoveSpeed);
	DOREPLIFETIME(ABaseProjectile, ProjectileDamage);
	DOREPLIFETIME(ABaseProjectile, WeaponDataID);
}



// OnHit should be used to terminate the projectile when collising with world object, rather than dealing damage.
void ABaseProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	UE_LOG(LogTemp, Warning, TEXT(" ABaseProjectile::OnHit  "));


	if (HitSoundEffect != nullptr) { UGameplayStatics::PlaySound2D(this, HitSoundEffect); }
	if (HitEffectParticle != nullptr) { HitEffectParticle->ActivateSystem(true); }

	ToggleEnabled(false);

	if (OwningPool != NULL) {
		if (World != nullptr) World->GetTimerManager().ClearTimer(ReturnToPoolTimer);
		if (World != nullptr) World->GetTimerManager().SetTimer(ReturnToPoolTimer, this, &ABaseProjectile::ResetProjectile, 4.f);
	}

	//IDamageableInterface* ObjectInterface = Cast<IDamageableInterface>(OtherActor);

	//if (ObjectInterface) {

	//	if (WeaponData != nullptr) {

	//		// Attempt to Deal damage, and check whether it was successful.
	//		if (IDamageableInterface::Execute_RecieveDamage(OtherActor, (*WeaponData)->BaseWeaponDamage)) {

	//			if (HitSoundEffect != nullptr) { UGameplayStatics::PlaySound2D(this, HitSoundEffect); }
	//			if (HitEffectParticle != nullptr) { HitEffectParticle->ActivateSystem(true); }

	//			ToggleEnabled(false);

	//			if (OwningPool != NULL) {
	//				//SetProjectileLocationAndDirection(FVector(0, 0, 0), FVector(0, 0, 0), false); // this doesn't appear to be working
	//				//OwningPool->ReturnReusableReference(this);
	//				if (World != nullptr) World->GetTimerManager().ClearTimer(ReturnToPoolTimer);
	//				if (World != nullptr) World->GetTimerManager().SetTimer(ReturnToPoolTimer, this, &ABaseProjectile::ResetProjectile, 4.f);
	//			}

	//		}
	//		else  UE_LOG(LogTemp, Warning, TEXT(" ABaseProjectile::OnHit Unable to Deal damage  "));

	//	}



	//	if (Role == ROLE_Authority) {
	//		//if (WeaponData != nullptr) BroadcastHit(OtherActor, (*WeaponData)->BaseWeaponDamage);


	//	}
	//}


}

void ABaseProjectile::OnComponentEnterTrigger(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	
	UE_LOG(LogTemp, Warning, TEXT(" ABaseProjectile ::OnComponentEnterTrigger  "));

	IDamageableInterface* ObjectInterface = Cast<IDamageableInterface>(OtherActor);
	if (ObjectInterface) {

		if (WeaponData != nullptr) {

			// Attempt to Deal damage, and check whether it was successful.
			if ( IDamageableInterface::Execute_RecieveDamage( OtherActor, (*WeaponData)->BaseWeaponDamage, (*WeaponData)->OwningTeam ) ){

				/*
				if (HitSoundEffect != nullptr) { UGameplayStatics::PlaySound2D(this, HitSoundEffect); }
				if (HitEffectParticle != nullptr) { HitEffectParticle->ActivateSystem(true); }

				ToggleEnabled(false);

				if (OwningPool != NULL) {
					//SetProjectileLocationAndDirection(FVector(0, 0, 0), FVector(0, 0, 0), false); // this doesn't appear to be working
					//OwningPool->ReturnReusableReference(this);
					if (World != nullptr) World->GetTimerManager().ClearTimer(ReturnToPoolTimer);
					if (World != nullptr) World->GetTimerManager().SetTimer(ReturnToPoolTimer, this, &ABaseProjectile::ResetProjectile, 4.f);
				}
				*/

				MulticastTerminateProjectile();

			}
			else {
				UE_LOG(LogTemp, Warning, TEXT(" ABaseProjectile ::OnComponentEnterTrigger  Unable to Deal damage  "));
			}
		}
		//UE_LOG(LogTemp, Warning, TEXT(" ABaseProjectile ::OnComponentEnterTrigger  "));
		//if (Role == ROLE_Authority) {
		//	if (WeaponData != nullptr) BroadcastHit(OtherActor, (*WeaponData)->BaseWeaponDamage);

		//		IDamageableInterface* ObjectInterface = Cast<IDamageableInterface>(OtherActor);
		//	
		//		if (ObjectInterface) {
		//	
		//			IDamageableInterface::Execute_RecieveDamage(OtherActor, (*WeaponData)->BaseWeaponDamage);
		//	
		//		}
		//}

		//if (HitSoundEffect != nullptr) { UGameplayStatics::PlaySound2D(this, HitSoundEffect); }
		//if (HitEffectParticle != nullptr) { HitEffectParticle->ActivateSystem(true); }

		//ToggleEnabled(false);

		///*

		//if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL)) {
		//if (OtherActor->IsA(AEnemy::StaticClass())) {
		//Cast<AEnemy>(OtherActor)->ReceiveDamage(ProjectileDamage);
		//UE_LOG(LogTemp, Log, TEXT("HIT ENEMY"));
		//}
		//else {
		//UE_LOG(LogTemp, Log, TEXT("Hit Something else"));
		//}
		//}
		//*/
		//// Only add impulse and destroy projectile if we hit a physics
		///*
		//if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
		//{
		//OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		//}
		//*/


		//if (OwningPool != NULL) {
		//	//SetProjectileLocationAndDirection(FVector(0, 0, 0), FVector(0, 0, 0), false); // this doesn't appear to be working
		//	//OwningPool->ReturnReusableReference(this);
		//	if (World != nullptr) World->GetTimerManager().ClearTimer(ReturnToPoolTimer);
		//	if (World != nullptr) World->GetTimerManager().SetTimer(ReturnToPoolTimer, this, &ABaseProjectile::ResetProjectile, 4.f);
		//}
	}
}

void ABaseProjectile::OnComponentExitTrigger(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex) {
	//...

}




/*
Enables and disabled the projectile. Used for getting it from and returning it too the object pool and
when restarting the projectile following a collission or other similar event.
In order to enable the projectile over the network, this method should  be called before applying other changes.
*/
void ABaseProjectile::ToggleEnabled(bool _value) {

	if (_value) { // enable the projectile

				 // UE_LOG(LogTemp, Log, TEXT(" Projectile::Toggle enabled true"));

		ProjectileCollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ProjectileCollisionSphere->SetVisibility(true);
		ProjectileParticle->ActivateSystem(true);
		ProjectileMovement->Activate();
		HitEffectParticle->DeactivateSystem();
		ProjectileMovement->SetUpdatedComponent(GetRootComponent()); // moved to constructor

		if (World != nullptr) World->GetTimerManager().SetTimer(ReturnToPoolTimer, this, &ABaseProjectile::ResetProjectile, 4.f);

	}
	else { // disable the projectile

		  // UE_LOG(LogTemp, Log, TEXT(" Projectile::Toggle enabled false"));

		ProjectileCollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ProjectileCollisionSphere->SetVisibility(false);
		ProjectileParticle->DeactivateSystem();// Deactivate the particle. It may be the case that the particle will remain if lifetime = 0. If so, check KillOnDeactivate in the particle system.
		//HitEffectParticle->DeactivateSystem();
		ProjectileMovement->Deactivate();
	}
}

/*
Updates the location and velocity of the projectile.
This is the intended method for firing the projectile from the weapon component.
*/
void ABaseProjectile::SetProjectileLocationAndDirection(FVector _Loc, FVector _Vel, bool _ToggleEnabled) {
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(" SetProjectileLocationAndDirection. Server =: %s"), Role == ROLE_Authority ? TEXT("True") : TEXT("False")));
	if (Role == ROLE_Authority) {
		//UE_LOG(LogTemp, Log, TEXT(" SetProjectileLocationAndDirection, %f %f  %f"), _Loc.X, _Loc.Y, _Loc.Z);
		MulticastSetLocationAndVelocityDirection(_Loc, _Vel, _ToggleEnabled);
	}
}

void ABaseProjectile::PassNewWeaponData(FWeaponData _NewWeaponData, int _NewWeaponDataID) {

	//UE_LOG(LogTemp, Log, TEXT(" PassNewWeaponData old id = %d. new id = %d"), WeaponDataID, _NewWeaponDataID);
	if (Role == ROLE_Authority) {
		WeaponDataID = _NewWeaponDataID;

		ProjectileParticle->SetTemplate(_NewWeaponData.ProjectileParticleSystem);
		HitEffectParticle->SetTemplate(_NewWeaponData.HitEffectParticleSystem);
		HitSoundEffect = _NewWeaponData.HitSound;
		ProjectileDamage = _NewWeaponData.BaseWeaponDamage;
		CurrentMoveSpeed = _NewWeaponData.BaseProjectileSpeed;

		MulticastAssignWeaponDataValues(_NewWeaponData.ProjectileParticleSystem, _NewWeaponData.HitEffectParticleSystem, _NewWeaponData.HitSound, _NewWeaponData.BaseProjectileSpeed);
	}

	GetWorld()->GetAuthGameMode();

}

void ABaseProjectile::MulticastAssignNewWeaponData_Implementation(FWeaponData _NewWeaponData) {

	//UE_LOG(LogTemp, Log, TEXT("MulticastAssignNewWeaponData_Implementation. Components Size = %d"), _NewWeaponData.ProjectileComponents.Num());

	ProjectileParticle->SetTemplate(_NewWeaponData.ProjectileParticleSystem);
	HitEffectParticle->SetTemplate(_NewWeaponData.HitEffectParticleSystem);
	HitSoundEffect = _NewWeaponData.HitSound;
	ProjectileDamage = _NewWeaponData.BaseWeaponDamage;
	CurrentMoveSpeed = _NewWeaponData.BaseProjectileSpeed;
}

void ABaseProjectile::MulticastTerminateProjectile_Implementation() {
	
	if (HitSoundEffect != nullptr) { UGameplayStatics::PlaySound2D(this, HitSoundEffect); }
	if (HitEffectParticle != nullptr) { HitEffectParticle->ActivateSystem(true); }
	
	ToggleEnabled(false);

	if (OwningPool != NULL) {
		//SetProjectileLocationAndDirection(FVector(0, 0, 0), FVector(0, 0, 0), false); // this doesn't appear to be working
		//OwningPool->ReturnReusableReference(this);
		if (World != nullptr) World->GetTimerManager().ClearTimer(ReturnToPoolTimer);
		if (World != nullptr) World->GetTimerManager().SetTimer(ReturnToPoolTimer, this, &ABaseProjectile::ResetProjectile, 4.f);
	}
}


/*
For some reason, the weapondata must be broken up into its individual value types in order to pass them to the client, or else they
*/
void ABaseProjectile::MulticastAssignWeaponDataValues_Implementation(UParticleSystem* _NewParticleSystem, UParticleSystem* _NewHitParticleSystem, USoundBase* _HitSound, float _NewSpeed) {
	ProjectileParticle->SetTemplate(_NewParticleSystem);
	HitEffectParticle->SetTemplate(_NewHitParticleSystem);
	HitSoundEffect = _HitSound;
	//ProjectileDamage = _BaseWeaponDamage;
	CurrentMoveSpeed = _NewSpeed;

}
/*
Updates the projectiles location and velocity across the network.
*/
void ABaseProjectile::MulticastSetLocationAndVelocityDirection_Implementation(FVector _Loc, FVector _Vel, bool _ToggleEnabled) {
	ToggleEnabled(_ToggleEnabled); // re-enable movement component and restart particle system.
	SetActorLocation(_Loc);	// set the location of the projectile on the client(s) and server
	ProjectileMovement->Velocity = _Vel * CurrentMoveSpeed;
}

void ABaseProjectile::ResetProjectile() {
	if (OwningPool != NULL) {
		SetProjectileLocationAndDirection(FVector(0, 0, 0), FVector(0, 0, 0), false); // this doesn't appear to be working
		OwningPool->ReturnReusableReference(this);
	}
}


/*
Updates the velocity o the projectile on cliets. Called whenever a change is made to the velocity.
Only works with bReplicateMovement.....
*/
void  ABaseProjectile::PostNetReceiveVelocity(const FVector& NewVelocity) {
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(" ToggleEnabled. Server =: %s"), Role == ROLE_Authority ? TEXT("True") : TEXT("False")));

	UE_LOG(LogTemp, Log, TEXT(" PostNetReceiveVelocity %f %f %f"), NewVelocity.X, NewVelocity.Y, NewVelocity.Z);
	ProjectileMovement->Velocity = NewVelocity;
}