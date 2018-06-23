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

#include "Engine/World.h"

#include "DrawDebugHelpers.h"


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

const FName APlayerPaperCharacter::MoveUpBinding("VerticalAxis");
const FName APlayerPaperCharacter::MoveRightBinding("HorizontalAxis");
const FName APlayerPaperCharacter::FireUpBinding("ShootVerticalAxis");
const FName APlayerPaperCharacter::FireRightBinding("ShootHorizontalAxis");


APlayerPaperCharacter::APlayerPaperCharacter() {

	// Rotate the Sprite to face the negative X direction and tilt up to face the camera
	GetSprite()->AddLocalRotation(FRotator(00.f, 90.f, -30.f));

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

	//Create a shadow decal component beneath the character
	ShadowDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("ShadowDecal"));
	ShadowDecal->SetupAttachment(RootComponent);
	ShadowDecal->RelativeRotation = FRotator(90.0f, 0.0f, 0.0f);
	ShadowDecal->RelativeLocation = FVector(0.0f, 0.0f, -80.f);
	ShadowDecal->DecalSize = FVector(30.f, 50.f, 30.f);

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	bReplicates = true;

}

void APlayerPaperCharacter::SetupPlayerInputComponent(UInputComponent * InputComponent) {
	Super::SetupPlayerInputComponent(InputComponent);
	InputComponent->BindAxis(MoveUpBinding, this, &APlayerPaperCharacter::MoveUp);
	InputComponent->BindAxis(MoveRightBinding, this, &APlayerPaperCharacter::MoveRight);
	InputComponent->BindAxis(FireUpBinding);
	InputComponent->BindAxis(FireRightBinding);

}

void APlayerPaperCharacter::BeginPlay() {
	Super::BeginPlay();
	World = GetWorld();
}


//enum compassDir {
//	E = 0, NE = 1,
//	N = 2, NW = 3,
//	W = 4, SW = 5,
//	S = 6, SE = 7
//};


void APlayerPaperCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(Role), this, FColor::White, DeltaTime);


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
		const FString headings[4] = { "E", "N", "W", "S",};
		//const FString headings[8] = { "E", "NE", "N", "NW", "W", "SW", "S", "SE" };
		const int SizeOfHeadings = (sizeof(headings) / sizeof(*headings));
		float angle = FMath::Atan2(FireDirection.X, FireDirection.Y);
		int octant = FMath::RoundToInt(SizeOfHeadings * angle / (2 * PI) + SizeOfHeadings) % SizeOfHeadings;
		Heading dir = (Heading)octant;  // typecast to enum: 0 -> E etc.
		FString dirStr = headings[octant];
		//UE_LOG(LogTemp, Warning, TEXT(" %f , %f , Shoot dir = %s"), FireForwardValue, FireRightValue, *dirStr);

		ShootToHeading(dir);

	}

	UpdateAnimation();
}

void APlayerPaperCharacter::UpdateAnimation() {

	UPaperFlipbook* DesiredAnimation = nullptr;

	const FVector PlayerVelocity = GetVelocity();
	bool bIsMoving = PlayerVelocity.SizeSquared() > 0 ? true : false;

	//UE_LOG(LogTemp, Warning, TEXT("Player Velocity  =  %f , %f, %f"), PlayerVelocity.X, PlayerVelocity.Y, PlayerVelocity.Z);

	//TODO: Define a more appropriate state machine or see if one exists in unreal already for sprites

#pragma region Animation Conditions
	if (bIsShooting) { // Shooting Animation
		//UE_LOG(LogTemp, Warning, TEXT("bIsShooting "));
		switch (CurrentShootingDirection/*GetSHootingDirection()*/ ) {
		case EFaceDirection::FD_Left:
			if (bIsMoving) DesiredAnimation = WalkLeftFlipbook;
			else DesiredAnimation = IdleLeftFlipbook;
			break;
		case EFaceDirection::FD_Right:
			if (bIsMoving) DesiredAnimation = WalkRightFlipbook;
			else DesiredAnimation = IdleRightFlipbook;
			break;
		case EFaceDirection::FD_Up:
			if (bIsMoving) DesiredAnimation = WalkUpFlipbook;
			else DesiredAnimation = IdleUpFlipbook;
			break;
		case EFaceDirection::FD_Down:
			if (bIsMoving) DesiredAnimation = WalkDownFlipbook;
			else DesiredAnimation = IdleDownFlipbook;
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("bIsShooting switch FAILED"));
			break;
		}
	}
	else if (bIsMoving) { //Walking Animation
		//UE_LOG(LogTemp, Warning, TEXT("bIsMoving "));
		switch (CurrentMovingDirection) {
		case EFaceDirection::FD_Left:
			DesiredAnimation = WalkLeftFlipbook;
			break;
		case EFaceDirection::FD_Right:
			DesiredAnimation = WalkRightFlipbook;
			break;
		case EFaceDirection::FD_Up:
			DesiredAnimation = WalkUpFlipbook;
			break;
		case EFaceDirection::FD_Down:
			DesiredAnimation = WalkDownFlipbook;
			break;
		default :
			UE_LOG(LogTemp, Warning, TEXT("bIsMoving switch FAILED"));
			break;
		}
	}
	else { // Idle animation
		//UE_LOG(LogTemp, Warning, TEXT("Idle "));
		switch (CurrentMovingDirection) {
		case EFaceDirection::FD_Left:
			DesiredAnimation = IdleLeftFlipbook;
			break;
		case EFaceDirection::FD_Right:
			DesiredAnimation = IdleRightFlipbook;
			break;
		case EFaceDirection::FD_Up:
			DesiredAnimation = IdleUpFlipbook;
			break;
		case EFaceDirection::FD_Down:
			DesiredAnimation = IdleDownFlipbook;
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Idle switch FAILED"));
			break;
		}
	}
#pragma endregion

		//UPaperFlipbook* DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? RunningAnimation : IdleAnimation;
	if (DesiredAnimation != nullptr && GetSprite()->GetFlipbook() != DesiredAnimation) {

		GetSprite()->SetFlipbook(DesiredAnimation);
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


	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}


void APlayerPaperCharacter::MoveRight(float Value) {
	//if (MovementComponent == nullptr) return;
	if (Value > 0) {
		CurrentMovingDirection = EFaceDirection::FD_Right;
	}
	else if (Value < 0) {
		CurrentMovingDirection = EFaceDirection::FD_Left;
	}

	AddMovementInput(FVector(0.0f, 1.0f, 0.0f), Value);
}

//void APlayerPaperCharacter::ShootUp(float Value) {
//	if (Value > 0) {
//		CurrentShootingDirection = EFaceDirection::FD_Up;
//	}
//	else if (Value < 0) {
//		CurrentShootingDirection = EFaceDirection::FD_Down;
//	}
//	CurrentVerticalShootValue = Value;
//}
//
//void APlayerPaperCharacter::ShootRight(float Value) {
//	if (Value > 0) {
//		CurrentShootingDirection = EFaceDirection::FD_Right;
//	}
//	else if (Value < 0) {
//		CurrentShootingDirection = EFaceDirection::FD_Left;
//	}
//
//	CurrentHorizontalShootValue = Value;
//}


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

			// spawn the projectile
			ABaseProjectile* NewProjectile = World->SpawnActor<ABaseProjectile>(GetActorTransform().GetLocation(), FireRotation);
			//NewProjectile->OnEnemyHit.AddDynamic(this, &ASpaceApePlayerCharacter::DealDamage);

			//EquippedWeaponComponent->Shoot(_FireDirection);

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &APlayerPaperCharacter::ShotTimerExpired, 0.4f /*EquippedWeaponComponent->GetFireRate()*/);
		}
		//if (FireSound != nullptr) {
		//	MulticastPlayFireSound();
		//}
	}
}

void APlayerPaperCharacter::ShotTimerExpired() {
	bCanFire = true;
}
