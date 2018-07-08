// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPaperCharacter.h"

#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h" 
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "SpriteObjects/BaseProjectile.h"
#include "Components/SpriteShadowComponent.h"
#include "Components/ObjectPoolComponent.h"
#include "Components/PaperCharacterAnimationComponent.h"
#include "Components/BaseWeaponComponent.h"

#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"

#include "SpriteObjects/EnemyPaperCharacter.h"


FString GetEnumText(ENetRole Role) {
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "ERROR";
	}
}

FString GetEnumText(EFaceDirection Role) {
	switch (Role)
	{
	case EFaceDirection::FD_Left:
		return "FD_Left";
	case EFaceDirection::FD_Right:
		return "FD_Right";
	case EFaceDirection::FD_Up:
		return "FD_Up";
	case EFaceDirection::FD_Down:
		return "FD_Down";
	default:
		return "ERROR";
	}
}

const FName APlayerPaperCharacter::MoveUpBinding("VerticalAxis");
const FName APlayerPaperCharacter::MoveRightBinding("HorizontalAxis");
const FName APlayerPaperCharacter::FireUpBinding("ShootVerticalAxis");
const FName APlayerPaperCharacter::FireRightBinding("ShootHorizontalAxis");


APlayerPaperCharacter::APlayerPaperCharacter() {

	// Rotate the Sprite to face the negative X direction and tilt up to face the camera
	GetSprite()->AddLocalRotation(FRotator(00.f, 90.f, -30.f));
	// Enable replication on the Sprite component so animations show up when networked
	//GetSprite()->SetIsReplicated(true);

	GetCapsuleComponent()->SetCollisionProfileName("Player");

	// Create a camera boom attached to the root (capsule) 
		//TODO: Replace camera component with a Level Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 120.0f);
	//CameraBoom->bAbsoluteRotation = true;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->RelativeRotation = FRotator(-40.0f, 0.0f, 0.0f);

	// Create an orthographic camera (no perspective) and attach it to the boom
	ObliqueViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ObliqueViewCamera"));
	//ObliqueViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	ObliqueViewCameraComponent->OrthoWidth = 2048.0f;
	ObliqueViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	//CameraBoom->bAbsoluteRotation = true;
	ObliqueViewCameraComponent->bUsePawnControlRotation = false;
	ObliqueViewCameraComponent->bAutoActivate = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	ShadowComponent = CreateDefaultSubobject<USpriteShadowComponent>(TEXT("ShadowComponent"));

	AnimationComponent = CreateDefaultSubobject<UPaperCharacterAnimationComponent>(TEXT("AnimationComponent"));

	ProjectilePool = CreateDefaultSubobject<UObjectPoolComponent>(TEXT("ProjectilePool"));

	//SetOwner(GetController());

	bReplicates = true;

}

void APlayerPaperCharacter::SetupPlayerInputComponent(UInputComponent * InputComponent) {
	Super::SetupPlayerInputComponent(InputComponent);
	InputComponent->BindAxis(MoveUpBinding, this, &APlayerPaperCharacter::MoveUp);
	InputComponent->BindAxis(MoveRightBinding, this, &APlayerPaperCharacter::MoveRight);
	InputComponent->BindAxis(FireUpBinding, this, &APlayerPaperCharacter::ShootUp);
	InputComponent->BindAxis(FireRightBinding, this , &APlayerPaperCharacter::ShootRight);
}

void APlayerPaperCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(APlayerPaperCharacter, bIsShooting);
	DOREPLIFETIME(APlayerPaperCharacter, CurrentMovingDirection);
	DOREPLIFETIME(APlayerPaperCharacter, CurrentShootingDirection);
}

void APlayerPaperCharacter::BeginPlay() {
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
			Cast<ABaseProjectile>(Actor)->OnEnemyHit.AddDynamic(this, &APlayerPaperCharacter::DealDamage);
			Cast<ABaseProjectile>(Actor)->SetPoolReference(ProjectilePool);
		}
	}
}


	void APlayerPaperCharacter::Tick(float DeltaTime) {
		Super::Tick(DeltaTime);

		DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(Role), this, FColor::White, DeltaTime - 0.1f);

		DrawDebugString(GetWorld(), FVector(0, 0, 85), GetEnumText(CurrentMovingDirection), this, FColor::Emerald, DeltaTime - 0.1f);

		HandleMovement(DeltaTime);
		HandleShooting();

		if (LastUpdatedMovingDirection != CurrentMovingDirection && Role == ROLE_AutonomousProxy) {
			ServerModifyMoveDirection(this, CurrentMovingDirection);
			LastUpdatedMovingDirection = CurrentMovingDirection;
		}

		//if (HasAuthority()) {

			//ReplicatedMovingDirection = CurrentMovingDirection;
			//ReplicatedShootingDirection = CurrentShootingDirection;
		//}
	}


void APlayerPaperCharacter::HandleMovement(float DeltaTime) {

	// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
	FVector MoveDirection = FVector(CurrentVerticalMoveValue, CurrentHorizontalMoveValue, 0.f).GetClampedToMaxSize(1.0f);

	// Calculate  movement
	const FVector Movement = MoveDirection * /*MoveSpeed*/10 * DeltaTime;
	if (Movement.SizeSquared() > 0.0f) {
		//Apply Movement
		AddMovementInput(MoveDirection, 1);

		//UE_LOG(LogTemp, Warning, TEXT("HandleMovement: %f , %f"), CurrentVerticalMoveValue, CurrentHorizontalMoveValue);
	}
}

void APlayerPaperCharacter::OnRep_ReplicatedShootingDirection() {
	CurrentShootingDirection = ReplicatedShootingDirection;
}

void APlayerPaperCharacter::HandleShooting() {

	bIsShooting = CurrentHorizontalShootValue != 0 || CurrentVerticalShootValue != 0;

	//if (CurrentHorizontalShootValue != 0 || CurrentVerticalShootValue != 0) bIsShooting = true;
	//else bIsShooting = false;

	//Twin stick shooting direction
	const float FireForwardValue = GetInputAxisValue(FireUpBinding);
	const float FireRightValue = GetInputAxisValue(FireRightBinding);

	//TODO: Refactor firing direction code for efficiency/ tidiness and reliability/ adaptability
	if (FireForwardValue != 0 || FireRightValue != 0) {
		const FVector FireDirection = FVector(FMath::RoundToInt(FireForwardValue), FMath::RoundToInt(FireRightValue), 0.0f); // round the values to int to counter 'analogue' input from joystick affecting magnitude
																															 //Cardinal Coordinate conversion
		const FString headings[4] = { "E", "N", "W", "S", };
		//const FString headings[8] = { "E", "NE", "N", "NW", "W", "SW", "S", "SE" };
		const int SizeOfHeadings = (sizeof(headings) / sizeof(*headings));
		float angle = FMath::Atan2(FireDirection.X, FireDirection.Y);
		int octant = FMath::RoundToInt(SizeOfHeadings * angle / (2 * PI) + SizeOfHeadings) % SizeOfHeadings;
		Heading dir = (Heading)octant;  // typecast to enum: 0 -> E etc.
		FString dirStr = headings[octant];
		//UE_LOG(LogTemp, Warning, TEXT(" %f , %f , Shoot dir = %s"), FireForwardValue, FireRightValue, *dirStr);
		if (bCanFire == true) {
			if (Role == ROLE_AutonomousProxy) {
				ServerShootToHeading(dir);
			}
			else if (Role == ROLE_Authority) {
				ShootToHeading(dir); // TODO: Test whether we need this fot the server player
			}
		}

	}
}

void APlayerPaperCharacter::MoveUp(float Value) {
	//if (MovementComponent == nullptr) return;

	if (Value > 0) {
		CurrentMovingDirection = EFaceDirection::FD_Up;
	}
	else if (Value < 0) {
		CurrentMovingDirection = EFaceDirection::FD_Down;
	}
	CurrentVerticalMoveValue = Value;

	//AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}


void APlayerPaperCharacter::MoveRight(float Value) {
	//if (MovementComponent == nullptr) return;
	if (Value > 0) {
		CurrentMovingDirection = EFaceDirection::FD_Right;
	}
	else if (Value < 0) {
		CurrentMovingDirection = EFaceDirection::FD_Left;
	}

	CurrentHorizontalMoveValue = Value;

	//AddMovementInput(FVector(0.0f, 1.0f, 0.0f), Value);
}

void APlayerPaperCharacter::ShootUp(float Value) {
	if (Value > 0) {
		CurrentShootingDirection = EFaceDirection::FD_Up;
	}
	else if (Value < 0) {
		CurrentShootingDirection = EFaceDirection::FD_Down;
	}
	CurrentVerticalShootValue = Value;
}

void APlayerPaperCharacter::ShootRight(float Value) {
	if (Value > 0) {
		CurrentShootingDirection = EFaceDirection::FD_Right;
	}
	else if (Value < 0) {
		CurrentShootingDirection = EFaceDirection::FD_Left;
	}

	CurrentHorizontalShootValue = Value;
}


void APlayerPaperCharacter::ShootToHeading(Heading HeadingDirection) {
	if (bCanFire == true) {

		if (World != NULL) {

			FVector FireDirection = FVector(0, 0, 0);

			switch (HeadingDirection) {
			case Heading::H_East:
				FireDirection = GetActorRightVector();
				break;
			case Heading::H_North:
				FireDirection = GetActorForwardVector();
				break;
			case Heading::H_West:
				FireDirection = -GetActorRightVector();
				break;
			case Heading::H_South:
				FireDirection = -GetActorForwardVector();
				break;
			default:
				break;
			}


			const FRotator FireRotation = FireDirection.Rotation();
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(FVector(0.5f, 0,0)/*GunOffset*/);

			if (EquippedWeaponComponent != nullptr) EquippedWeaponComponent->Shoot(FireDirection);
			else UE_LOG(LogTemp, Warning, TEXT("EquippedWeaponComponent == nullptr"));

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &APlayerPaperCharacter::ShotTimerExpired, 0.4f /*EquippedWeaponComponent->GetFireRate()*/);
		}
		if (FireSound != nullptr) {
			MulticastPlayFireSound();
		}
	}
}

void APlayerPaperCharacter::ServerShootToHeading_Implementation(Heading HeadingDirection) {
	UE_LOG(LogTemp, Warning, TEXT("ServerShootToHeading_Implementation"));
	if (bCanFire == true) {

		if (World != NULL) {

			FVector FireDirection = FVector(0, 0, 0);

			switch (HeadingDirection) {
			case Heading::H_East:
				FireDirection = GetActorRightVector();
				break;
			case Heading::H_North:
				FireDirection = GetActorForwardVector();
				break;
			case Heading::H_West:
				FireDirection = -GetActorRightVector();
				break;
			case Heading::H_South:
				FireDirection = -GetActorForwardVector();
				break;
			default:
				break;
			}


			const FRotator FireRotation = FireDirection.Rotation();
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(FVector(0.5f, 0, 0)/*GunOffset*/);

			if (EquippedWeaponComponent!= nullptr) EquippedWeaponComponent->Shoot(FireDirection);
			else UE_LOG(LogTemp, Warning, TEXT("EquippedWeaponComponent == nullptr"));

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &APlayerPaperCharacter::ShotTimerExpired, 0.4f /*EquippedWeaponComponent->GetFireRate()*/);
		}
		if (FireSound != nullptr) {
			MulticastPlayFireSound();
		}
	}
}

void APlayerPaperCharacter::ShotTimerExpired() {
	bCanFire = true;
}

void APlayerPaperCharacter::DealDamage(AActor* _Enemy, int _Amount) {
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(" DealDamage 2 param")));
	if (AEnemyPaperCharacter* Enemy = Cast<AEnemyPaperCharacter>(_Enemy)) {
		bool isEnemyDead;

		int scoreFromDamage;

		//Enemy->ReceiveDamage(EquippedWeaponComponent->GetWeaponData()->BaseWeaponDamage, isEnemyDead, scoreFromDamage);
		Enemy->ReceiveDamage(_Amount, isEnemyDead, scoreFromDamage);

		if (isEnemyDead) {
			//scoreFromDamage += Enemy->GetScoreValue();
		}

		//CurrentScore += scoreFromDamage;



		// Inform the enemy of damage and check whether the enemy died as a result.
		//if (Enemy->ReceiveDamage(PlayerProjectileDamag EquippedWeaponComponent->GetWeaponData().BaseWeaponDamage, isEnemyDead, scoreFromDamage)) { // enemy recieve damage could return the damage done (int) in order to add that to the score
		//	CurrentScore += ScoreValue;
		//}
		//else CurrentScore += scoreFromDamage;
	}

	//GetController()->PlayerState->Score = CurrentScore; // needs storing. Could pass this to the server to validate

														//NOTE: It isn't safe to store score on the actor, as it could potentially be cheated.


}

void APlayerPaperCharacter::MulticastPlayFireSound_Implementation() {
	if (FireSound != nullptr) {
		UGameplayStatics::PlaySound2D(this, FireSound);
		//UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
}

/*
This will be used primarily for weapon pickups.
Replaces the current weapon witha  new o ne.
Need to destroy old component and perhaps perform some kind of check.
This method should eventually be made private/ protected, and some kind of public condition check method should handle weapon changes.
*/
void APlayerPaperCharacter::ChangeWeapon(TSubclassOf<UBaseWeaponComponent> _NewWeapon) {

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(" ChangeWeapon. Server =: %s"), Role == ROLE_Authority ? TEXT("True") : TEXT("False")));

	if (_NewWeapon) {

		//EquippedWeaponComponent = ConstructObject<UUBaseWeaponComponent>(_NewWeapon, this, *_NewWeapon->GetName()/*TEXT("InitialWeapon")*/);
		if (EquippedWeaponComponent != nullptr) EquippedWeaponComponent->DestroyComponent();
		EquippedWeaponComponent = NewObject<UBaseWeaponComponent>(this, _NewWeapon, *_NewWeapon->GetName());
		EquippedWeaponComponent->SetObjectPoolReference(ProjectilePool);

		EquippedWeaponComponent->RegisterComponent(); // this has been added to enable the component tick for some components
		EquippedWeaponData = EquippedWeaponComponent->GetWeaponData();
		//delete FireSound;
		FireSound = EquippedWeaponComponent->GetFireSound();

		//FWeaponData NewWeaponData = EquippedWeaponComponent->GetWeaponData();
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Emerald, FString::Printf(TEXT(" GetWeaponData Speed =  %f. Is Server = %s"), NewWeaponData.BaseProjectileSpeed, Role == ROLE_Authority ? TEXT("True") : TEXT("False")));
	}

}