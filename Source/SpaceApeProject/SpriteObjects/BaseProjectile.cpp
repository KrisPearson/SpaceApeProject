// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseProjectile.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"
//#include "Components/BaseProjectileComponent.h"
#include "Net/UnrealNetwork.h"

#include "Engine.h"


//TODO: Consider whether enemies need their own projectile class - could both inherrit from same base

// Sets default values
ABaseProjectile::ABaseProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ProjectileParticleAsset(TEXT("/Game/Particles/Test_ParticleSystem"));

	// Colisssion Mesh
	ProjectileMesh = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetVisibility(true);
	ProjectileMesh->SetEnableGravity(false);
	//ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("PlayerProjectile"); //TODO: Consider how best to change colission type (two different child classes?)
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnHit);
	RootComponent = ProjectileMesh;

	// Movement Component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 1000.f;
	ProjectileMovement->MaxSpeed = 15000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->Bounciness = 1.0f; // Do not lose velocity when bounces.
	ProjectileMovement->SetUpdatedComponent(GetRootComponent()); // Ensures that the root component is updated by the projectile movement component

	//Particle System
	ProjectileParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileParticle"));
	ProjectileParticle->SetTemplate(ProjectileParticleAsset.Object);
	ProjectileParticle->SetupAttachment(RootComponent);


}
void ABaseProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	World = GetWorld();
}

// Called every frame
void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	UE_LOG(LogTemp, Warning, TEXT(" ABaseProjectile::OnHit"));
	if (Role == ROLE_Authority) {
		//if (WeaponData != nullptr) BroadcastHit(OtherActor, (*WeaponData)->BaseWeaponDamage);
		BroadcastHit(OtherActor, 10 /*Damage*/);

		//for (UBaseProjectileComponent* Component : ProjectileComponents) {
		//	Component->ExecuteHitEvent();
		//}
	}

	//if (HitSoundEffect != nullptr) { UGameplayStatics::PlaySound2D(this, HitSoundEffect); }
	//if (HitEffectParticle != nullptr) { HitEffectParticle->ActivateSystem(true); }

	//ToggleEnabled(false);

	/*

	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL)) {
	if (OtherActor->IsA(AEnemy::StaticClass())) {
	Cast<AEnemy>(OtherActor)->ReceiveDamage(ProjectileDamage);
	UE_LOG(LogTemp, Log, TEXT("HIT ENEMY"));
	}
	else {
	UE_LOG(LogTemp, Log, TEXT("Hit Something else"));
	}
	}
	*/
	// Only add impulse and destroy projectile if we hit a physics
	/*
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
	OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}
	*/


	//if (OwningPool != NULL) {
		//SetProjectileLocationAndDirection(FVector(0, 0, 0), FVector(0, 0, 0), false); // this doesn't appear to be working
		//OwningPool->ReturnReusableReference(this);
		//if (World != nullptr) World->GetTimerManager().ClearTimer(ReturnToPoolTimer);
		//if (World != nullptr) World->GetTimerManager().SetTimer(ReturnToPoolTimer, this, &ASpaceApeProjectile::ResetProjectile, 4.f);
	//}
}
