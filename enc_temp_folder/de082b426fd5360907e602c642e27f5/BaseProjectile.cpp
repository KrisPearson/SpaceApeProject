// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "Engine.h"


// Sets default values
ABaseProjectile::ABaseProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	// Colisssion Mesh
	ProjectileMesh = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileMesh"));
	//ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	//ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	//ProjectileMesh->OnComponentHit.AddDynamic(this, &ASpaceApeProjectile::OnHit);
	RootComponent = ProjectileMesh;

	// Movement Component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	//ProjectileMovement->InitialSpeed = 2000.f;
	//ProjectileMovement->MaxSpeed = 15000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->Bounciness = 1.0f; // Do not lose velocity when bounces.
	ProjectileMovement->SetUpdatedComponent(GetRootComponent()); // Ensures that the root component is updated by the projectile movement component




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

