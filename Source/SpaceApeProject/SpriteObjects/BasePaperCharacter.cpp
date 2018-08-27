// // Copyright 2018 Kristiam Pearson. All Rights Reserved.

#include "BasePaperCharacter.h"
#include "Net/UnrealNetwork.h"

#include "Components/BaseWeaponComponent.h"
#include "Components/ObjectPoolComponent.h"
#include "Components/PaperCharacterAnimationComponent.h"
#include "Components/SpriteShadowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CollissionDamageComponent.h"
#include "PaperFlipbookComponent.h"

#include "Enums/TeamOwnerEnum.h"

#include "Materials/MaterialInstance.h"
#include "AIController.h"

#include "SpriteObjects/BaseProjectile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Kismet/GameplayStatics.h"



ABasePaperCharacter::ABasePaperCharacter() {

	ShadowComponent = CreateDefaultSubobject<USpriteShadowComponent>(TEXT("ShadowComponent"));

	AnimationComponent = CreateDefaultSubobject<UPaperCharacterAnimationComponent>(TEXT("AnimationComponent"));

	ProjectilePool = CreateDefaultSubobject<UObjectPoolComponent>(TEXT("ProjectilePool"));

	PivotComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PivotComponent"));
	PivotComponent->SetupAttachment(RootComponent);
	PivotComponent->SetRelativeLocation(FVector(30, 0, -90));
	GetSprite()->SetupAttachment(PivotComponent);
	GetSprite()->SetRelativeRotation(FRotator(00.f, 90.f, CameraAngle));
	GetSprite()->SetRelativeLocation(FVector(40.f, 0.f, 30.f));


	FVector Origin;
	FVector BoxExtent;
	float SphereRadius = 0;

	if (GetCapsuleComponent() != NULL) {
		Origin = GetCapsuleComponent()->Bounds.Origin;
		BoxExtent = GetCapsuleComponent()->Bounds.BoxExtent;
		SphereRadius = GetCapsuleComponent()->Bounds.SphereRadius;
	}

	CollissionOverlapComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollissionOverlap"));
	CollissionOverlapComponent->AttachTo(RootComponent);
	CollissionOverlapComponent->SetBoxExtent(BoxExtent * 1.2f);
	//CollissionOverlapComponent->SetRelativeLocation(Origin);

	CollissionDamageComponent = CreateDefaultSubobject<UCollissionDamageComponent>(TEXT("DamageCollider"));
	CollissionDamageComponent->InitialiseComponent(*GetCapsuleComponent(), *CollissionOverlapComponent);

	
	//TeamOwner = TeamOwner::ETeamOwner::TO_NoOwner;
	SetGenericTeamId(1);

	if (Cast<AAIController>(GetController()) ) Cast<AAIController>(GetController())->SetGenericTeamId(GetGenericTeamId());

	bReplicates = true;
}


void ABasePaperCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasePaperCharacter, bIsShooting);
	//DOREPLIFETIME(ABasePaperCharacter, CurrentMovingDirection);
	DOREPLIFETIME(ABasePaperCharacter, CurrentShootingDirection);
	DOREPLIFETIME(ABasePaperCharacter, CurrentHealthPoints);

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
			//Cast<ABaseProjectile>(Actor)->OnEnemyHit.AddDynamic(this, &ABasePaperCharacter::DealDamage);
			Cast<ABaseProjectile>(Actor)->SetPoolReference(ProjectilePool);
		}


	}

	UMaterialInstance* BaseMat = static_cast<UMaterialInstance*>(GetSprite()->GetMaterial(0));

	//Create a dynamic material in order to enable access to the damage flicker scaler parameter.
	if (BaseMat != nullptr) {
		DynamicSpriteMaterial = UMaterialInstanceDynamic::Create(BaseMat, this);
		GetSprite()->SetMaterial(0, DynamicSpriteMaterial);

	}

	// Set the health points the class uses during play to the value of the character's default health points.
	CurrentHealthPoints = HealthPoints;

	//GetController()->SetIgnoreMoveInput(true);

	//IDamageableInterface* ObjectInterfaceTwo = Cast<IDamageableInterface>(this);
	//if (ObjectInterfaceTwo) {
	//	IDamageableInterface::Execute_RecieveDamage(this, 2, GetTeamOwner());
	//}


	//IGenericTeamAgentInterface* ObjectInterface = Cast<IGenericTeamAgentInterface>(this);
	//if (ObjectInterface) {

	//	SetGenericTeamId(IGenericTeamAgentInterface::Execute_GetGenericTeamId(this));
	//	IGenericTeamAgentInterface::Execute_GetGenericTeamId(this);
	//	//ObjectInterface->GetGenericTeamId();

	//	//if (IDamageableInterface::Execute_RecieveDamage(OtherActor, (*WeaponData)->BaseWeaponDamage, (*WeaponData)->OwningTeam)) {

	//}


	SetGenericTeamId(FGenericTeamId(1));

	UE_LOG(LogTemp, Warning, TEXT("teamID BaseCharacter = %d"), GetGenericTeamId().GetId());
}

void ABasePaperCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	UpdateFaceDirection();
	
	UpdateIsShooting();

	//HandleMovement(DeltaTime);

	HandleShooting();


	// TEMP: Used to force move in a direction  TODO:move to movement component
	if (bInputDisabled) AddMovementInput(OverrideDirection, 1); 

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

void ABasePaperCharacter::HandleShooting() {

	if (bIsShooting) {

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(" bIsShooting = true")));

		if (Role == ROLE_AutonomousProxy) {
			ServerShootWeapon(/*CurrentShootingDirection*/);
		}
		else if (Role == ROLE_Authority) {
			ShootWeapon(/*CurrentShootingDirection*/); // TODO: Test whether the server player needs to call this
		}
	}
}

void ABasePaperCharacter::UpdateFaceDirection() {

	FVector CurrentVelocity = GetVelocity();
	CurrentVelocity.Normalize();
	//UE_LOG(LogTemp, Warning, TEXT("Player Velocity  =  %f , %f, %f"), GetVelocity().X, GetVelocity().Y, GetVelocity().Z);
	//UE_LOG(LogTemp, Warning, TEXT("CurrentVelocity  =  %f , %f, %f"), CurrentVelocity.X, CurrentVelocity.Y, CurrentVelocity.Z);

	if (CurrentVelocity.SizeSquared() == 0) {
		// use last face direction
		if (bIsShooting) FaceDirectionVector = CurrentShootingDirection;
		return;
	}
	else if (FMath::Abs(CurrentVelocity.X) > FMath::Abs(CurrentVelocity.Y)) {
		// Vertical axis
		if (CurrentVelocity.X > 0) {
			//CurrentMovingDirection = EFaceDirection::FD_Up;
			FaceDirectionVector = SpriteDirection::Up;
		}
		else {
			//CurrentMovingDirection = EFaceDirection::FD_Down;
			FaceDirectionVector = SpriteDirection::Down;
		}
	}
	else {
		//Horizontal axis
		if (CurrentVelocity.Y > 0) {
			//CurrentMovingDirection = EFaceDirection::FD_Right;
			FaceDirectionVector = SpriteDirection::Right;
		}
		else {
			//CurrentMovingDirection = EFaceDirection::FD_Left;
			FaceDirectionVector = SpriteDirection::Left;
		}
	}
}

void ABasePaperCharacter::ShootInDirection(FVector DirectionToShoot) {
	ServerSetCurrentShootingDirection( DirectionToShoot);
	CurrentHorizontalShootValue = 1;
}

/* Establishes whether the character is shooting and, if so, replicates the bIsShooting property across to all players */
void ABasePaperCharacter::UpdateIsShooting() {
	if (IsLocallyControlled()) { // Only update if the character is owned by this player
		const bool newBIsShooting = CurrentHorizontalShootValue != 0 || CurrentVerticalShootValue != 0;
		if (newBIsShooting != bIsShooting) {
			bIsShooting = newBIsShooting; // Local update to prevent latency causing firing after input is realeased
			if (HasAuthority()) MulticastSetIsShooting(newBIsShooting); // Server player
			else if (Role == ROLE_AutonomousProxy) ServerSetIsShooting(newBIsShooting); // Client Player		
		}
	}
}



/*
Tells the material to flicker, following the enemy recieving damage.
Called on server, and runs on server and all clients.
*/
void ABasePaperCharacter::MulticastPlayDamageFlash_Implementation() {
	if (DynamicSpriteMaterial != nullptr) {
		if (World != nullptr) {
			// Send the current time to the material, which will then handle the duration of the flicker.
			// This approach means that the material's scaler value only needs to be set once, rather than updated by the AEnemy class each frame, or via Timer.
			DynamicSpriteMaterial->SetScalarParameterValue(FName("StartTime"), World->GetTimeSeconds());
		}
		else UE_LOG(LogTemp, Warning, TEXT(" Warning: World = Null"));
	}
	else  UE_LOG(LogTemp, Warning, TEXT(" Warning: DynamicEnemyMaterial = Null"));
}

/*
A check to see whether the enemy has health points remianing.
*/
bool ABasePaperCharacter::CheckIfAlive() {
	if (CurrentHealthPoints > 0) {
		return true;
	}
	else return false;
}



/*
Sends a request to the WeaponComponent to shoot
*/
void ABasePaperCharacter::ShootWeapon() {
	if (EquippedWeaponComponent != nullptr) EquippedWeaponComponent->Shoot(CurrentShootingDirection);
}

void ABasePaperCharacter::ServerShootWeapon_Implementation() {
	if (EquippedWeaponComponent != nullptr) EquippedWeaponComponent->Shoot(CurrentShootingDirection);
}



/*
Destroy this actor following a broadcast to the WaveManager.
*/
void ABasePaperCharacter::CharacterDeath() {
	//SpawnPickup(); 
	CharacterDeathDelegate.Broadcast(this);
	Destroy();
}


/*Informs the charatcer of a new set of room bounds.
@param The box component from which to source the new bounds.
*/
void ABasePaperCharacter::SetCurrentRoomBounds(const UBoxComponent & CameraBoundsBox) {
	if (Role == ROLE_Authority || Role == ROLE_AutonomousProxy) {
		FVector BoxExtent = CameraBoundsBox.GetScaledBoxExtent();

		FVector Origin = CameraBoundsBox.GetComponentLocation();

		MinMaxRoomBounds.Key = FVector(Origin.X - BoxExtent.X, Origin.Y - BoxExtent.Y, Origin.Z - BoxExtent.Z);
		MinMaxRoomBounds.Value = FVector(Origin.X + BoxExtent.X, Origin.Y + BoxExtent.Y, Origin.Z + BoxExtent.Z);

		UE_LOG(LogTemp, Warning, TEXT("Room Bounds Minx = %f , Max X = %f"), MinMaxRoomBounds.Key.X, MinMaxRoomBounds.Value.X);
	}
}

/*Forcefully overrides movement input to move the character to a target location */
void ABasePaperCharacter::ForceMoveToLocation(FVector TargetLocation) {

	FVector CurrentLocation = GetActorLocation();

	//TODO: Implement 'L' shape movement to handle walking around corners

	OverrideDirection = TargetLocation - CurrentLocation;


	float DistanceToTarget = FVector::Distance(CurrentLocation, TargetLocation);
	GetMovementComponent()->GetMaxSpeed();


	//float t = (GetMovementComponent()->GetMaxSpeed() - 0 ) / GetMovementComponent()->getAcc

	// TODO: Calculate time to move to location
	float TimerDuratrion = DistanceToTarget / GetMovementComponent()->GetMaxSpeed();
	TimerDuratrion += 0.1f;


	UE_LOG(LogTemp, Warning, TEXT(" ABasePaperCharacter::ForceMoveToLocation: %f , %f"), OverrideDirection.X, OverrideDirection.Y);
	UE_LOG(LogTemp, Warning, TEXT(" Time to move = %f "), TimerDuratrion );

	bInputDisabled = true;
	GetWorld()->GetTimerManager().SetTimer(MovementOverrideTimer, this, &ABasePaperCharacter::EnableCharacterInput, TimerDuratrion, false);



	// Calculate  movement
	//const FVector Movement = MoveDirection * /*MoveSpeed*/10 * DeltaTime;
	//if (Movement.SizeSquared() > 0.0f) {
	//	//Apply Movement
	//	AddMovementInput(MoveDirection, 1);

	//	//UE_LOG(LogTemp, Warning, TEXT("HandleMovement: %f , %f"), CurrentVerticalMoveValue, CurrentHorizontalMoveValue);
	//}
}

void ABasePaperCharacter::EnableCharacterInput() {
	bInputDisabled = false;
}


//void ABasePaperCharacter::DealDamage(AActor * ActorToDamage, float DamageAmount) {
//
//	IDamageableInterface* ObjectInterface = Cast<IDamageableInterface>(ActorToDamage);
//
//	if (ObjectInterface) {
//
//		IDamageableInterface::Execute_RecieveDamage(ActorToDamage, DamageAmount);
//
//	}
//}


#pragma region CharacterInterface Methods 



bool ABasePaperCharacter::RecieveDamage_Implementation(float DamageAmount, FGenericTeamId DamageFromTeam) {

	UE_LOG(LogTemp, Warning, TEXT("ABasePaperCharacter::RecieveDamage GenericTeamId = %d"), TeamId.GetId());

	if (DamageFromTeam != TeamId) { //TODO: Refine this system (profile predefined responses?)

		CurrentHealthPoints -= DamageAmount;

		if (!CheckIfAlive())
		{
			CharacterDeath();
		}
		else {
			MulticastPlayDamageFlash();
		}

		return true;

	}
	else return false;
}

void ABasePaperCharacter::SetGenericTeamId(const FGenericTeamId & TeamID) {
	TeamId = TeamID;
	if (auto* AiController = Cast<AAIController>(GetController())) AiController->SetGenericTeamId(TeamID);
}

FGenericTeamId ABasePaperCharacter::GetGenericTeamId() const {
	return TeamId;
}



FVector ABasePaperCharacter::GetObjectFaceDirection_Implementation() const {

	return FaceDirectionVector;
}


//FVector ABasePaperCharacter::GetObjectFaceDirection_Implementation() const {
//	check(0 && "GetObjectFaceDirection_Implementation method requires override  method")
//	return FVector();
//}

//
//bool ABasePaperCharacter::DealDamage_Implementation(ABasePaperCharacter * CharacterToDamage, int _DamageAmount) {
//
//	return true;
//}

#pragma endregion
