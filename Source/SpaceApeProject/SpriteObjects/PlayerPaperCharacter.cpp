// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPaperCharacter.h"

#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h" 

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

#include "Components/DecalComponent.h"
//#include "SpriteObjects/BaseProjectile.h"
//#include "Components/SpriteShadowComponent.h"
#include "Components/ObjectPoolComponent.h"
//#include "Components/PaperCharacterAnimationComponent.h"
#include "Components/BaseWeaponComponent.h"
#include "Components/PlayerCameraControllerComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Engine/World.h"

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

	// Enable replication on the Sprite component so animations show up when networked
	//GetSprite()->SetIsReplicated(true);

	GetCapsuleComponent()->SetCollisionProfileName("Player");

	// Create a camera boom attached to the root (capsule) 
	CameraBoomComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoomComponent->SetupAttachment(RootComponent);
	CameraBoomComponent->TargetArmLength = 500.0f;
	//CameraBoomComponent->SocketOffset = FVector(0.0f, 0.0f, 120.0f);
	//CameraBoom->bAbsoluteRotation = true;
	CameraBoomComponent->bDoCollisionTest = false;
	CameraBoomComponent->RelativeRotation = FRotator(CameraAngle, 0.0f, 0.0f);

	// Create an orthographic camera (no perspective) and attach it to the boom
	ObliqueViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ObliqueViewCamera"));
	//ObliqueViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	ObliqueViewCameraComponent->OrthoWidth = 2048.0f;
	ObliqueViewCameraComponent->SetupAttachment(CameraBoomComponent, USpringArmComponent::SocketName);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	//CameraBoom->bAbsoluteRotation = true;
	ObliqueViewCameraComponent->bUsePawnControlRotation = false;
	ObliqueViewCameraComponent->bAutoActivate = true;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	GetSprite()->AddLocalRotation(FRotator(00.f, 90.f, CameraAngle));

	CameraController = CreateDefaultSubobject<UPlayerCameraControllerComponent>(TEXT("CameraController"));
	CameraController->SetBoomReference(*CameraBoomComponent);


	//SetOwner(GetController());

}



void APlayerPaperCharacter::SetupPlayerInputComponent(UInputComponent * InputComponent) {
	Super::SetupPlayerInputComponent(InputComponent);
	InputComponent->BindAxis(MoveUpBinding, this, &APlayerPaperCharacter::MoveUp);
	InputComponent->BindAxis(MoveRightBinding, this, &APlayerPaperCharacter::MoveRight);
	InputComponent->BindAxis(FireUpBinding, this, &APlayerPaperCharacter::ShootUp);
	InputComponent->BindAxis(FireRightBinding, this , &APlayerPaperCharacter::ShootRight);
}


void APlayerPaperCharacter::BeginPlay() {
	Super::BeginPlay();

}


void APlayerPaperCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(Role), this, FColor::White, DeltaTime - 0.1f);

	//DrawDebugString(GetWorld(), FVector(0, 0, 85), GetEnumText(CurrentMovingDirection), this, FColor::Emerald, DeltaTime - 0.1f);


	HandleMovement(DeltaTime);
	HandleShooting();

	//if (LastUpdatedMovingDirection != CurrentMovingDirection && Role == ROLE_AutonomousProxy) {
	//	ServerModifyMoveDirection(this, CurrentMovingDirection);
	//	LastUpdatedMovingDirection = CurrentMovingDirection;
	//}


	// TODO: Refactor the forward vector seting and use an interface to retrieve it from the player character (SpriteObjectInterface.h)

	//if (bIsShooting == true) {
	//	switch (CurrentShootingDirection) {
	//	case EFaceDirection::FD_Left:
	//		 FaceDirectionVector = FVector(0, -1, 0);
	//		break;
	//	case EFaceDirection::FD_Right:
	//		 FaceDirectionVector = FVector(0, 1, 0);
	//		break;
	//	case EFaceDirection::FD_Up:
	//		 FaceDirectionVector = FVector(1, 0, 0);
	//		break;
	//	case EFaceDirection::FD_Down:
	//		 FaceDirectionVector = FVector(-1, 0, 0);
	//		break;
	//	default:
	//		break;
	//	}
	//}
	//else {
	//	switch (CurrentMovingDirection) {
	//	case EFaceDirection::FD_Left:
	//		 FaceDirectionVector = FVector(0, -1, 0);
	//		break;
	//	case EFaceDirection::FD_Right:
	//		 FaceDirectionVector = FVector(0, 1, 0);
	//		break;
	//	case EFaceDirection::FD_Up:
	//		 FaceDirectionVector = FVector(1, 0, 0);
	//		break;
	//	case EFaceDirection::FD_Down:
	//		 FaceDirectionVector = FVector(-1, 0, 0);
	//		break;
	//	default:
	//		break;
	//	}
	//}


}


void APlayerPaperCharacter::HandleMovement(float DeltaTime) {

	if (bInputDisabled) return; //TEMP

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



void APlayerPaperCharacter::HandleShooting() {

	//else if (Role == ROLE_AutonomousProxy)

	//if (CurrentHorizontalShootValue != 0 || CurrentVerticalShootValue != 0) bIsShooting = true;
	//else bIsShooting = false;

	//Twin stick shooting direction
	//const float FireForwardValue = GetInputAxisValue(FireUpBinding);
	//const float FireRightValue = GetInputAxisValue(FireRightBinding);

	if (bIsShooting) {

		if (bCanFire == true) {
			if (Role == ROLE_AutonomousProxy) {
				ServerShootInDirection(CurrentShootingDirection);
			}
			else if (Role == ROLE_Authority) {
				ShootInDirection(CurrentShootingDirection); // TODO: Test whether we need this fot the server player
			}
		}

	}
}

void APlayerPaperCharacter::MoveUp(float Value) {
	CurrentVerticalMoveValue = Value;
}

void APlayerPaperCharacter::MoveRight(float Value) {
	CurrentHorizontalMoveValue = Value;
}

void APlayerPaperCharacter::ShootUp(float Value) {
	if (Value > 0) {
		CurrentShootingDirection = SpriteDirection::Up;
		ServerSetCurrentShootingDirection(SpriteDirection::Up);//TEMP
	}
	else if (Value < 0) {
		CurrentShootingDirection = SpriteDirection::Down;
		ServerSetCurrentShootingDirection(SpriteDirection::Down);//TEMP
	}
	CurrentVerticalShootValue = Value;
}

void APlayerPaperCharacter::ShootRight(float Value) {
	if (Value > 0) {
		CurrentShootingDirection = SpriteDirection::Right;
		ServerSetCurrentShootingDirection(SpriteDirection::Right);//TEMP
	}
	else if (Value < 0) {
		CurrentShootingDirection = SpriteDirection::Left;
		ServerSetCurrentShootingDirection(SpriteDirection::Left);//TEMP
	}

	CurrentHorizontalShootValue = Value;
}


void APlayerPaperCharacter::ShootInDirection(FVector Direction) {
	if (bCanFire == true) {

		if (World != NULL) {

			const FRotator FireRotation = Direction.Rotation();
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(FVector(0.5f, 0,0)/*GunOffset*/);

			if (EquippedWeaponComponent != nullptr) EquippedWeaponComponent->Shoot(Direction);
			else UE_LOG(LogTemp, Warning, TEXT("EquippedWeaponComponent == nullptr"));

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &APlayerPaperCharacter::ShotTimerExpired, EquippedWeaponComponent->GetFireRate() );
		}
		if (FireSound != nullptr) {
			MulticastPlayFireSound();
		}
	}
}

void APlayerPaperCharacter::ServerShootInDirection_Implementation(FVector Direction) {
	//UE_LOG(LogTemp, Warning, TEXT("ServerShootInDirection_Implementation"));
	if (bCanFire == true) {

		if (World != NULL) {

			const FRotator FireRotation = Direction.Rotation();
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(FVector(0.5f, 0, 0)/*GunOffset*/); // TODO: Utilise sockets for spawn location? How would sockets be updated to match rotation?

			if (EquippedWeaponComponent!= nullptr) EquippedWeaponComponent->Shoot(Direction);
			else UE_LOG(LogTemp, Warning, TEXT("EquippedWeaponComponent == nullptr"));

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &APlayerPaperCharacter::ShotTimerExpired, EquippedWeaponComponent->GetFireRate());
		}
		if (FireSound != nullptr) {
			MulticastPlayFireSound();
		}
	}
}

void APlayerPaperCharacter::ShotTimerExpired() {
	bCanFire = true;
}

void APlayerPaperCharacter::DealDamage(AActor * ActorToDamage, int DamageAmount) {

	Super::DealDamage(ActorToDamage, DamageAmount);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(" DealDamage 2 param")));
	//if (AEnemyPaperCharacter* Enemy = Cast<AEnemyPaperCharacter>(CharacterToDamage)) {
		//bool isEnemyDead;

		//int scoreFromDamage;

		//Enemy->ReceiveDamage(EquippedWeaponComponent->GetWeaponData()->BaseWeaponDamage, isEnemyDead, scoreFromDamage);
		//Enemy->ReceiveDamage(DamageAmount, isEnemyDead, scoreFromDamage);

		//if (isEnemyDead) {
			//scoreFromDamage += Enemy->GetScoreValue();
	//	}

		//CurrentScore += scoreFromDamage;



		// Inform the enemy of damage and check whether the enemy died as a result.
		//if (Enemy->ReceiveDamage(PlayerProjectileDamag EquippedWeaponComponent->GetWeaponData().BaseWeaponDamage, isEnemyDead, scoreFromDamage)) { // enemy recieve damage could return the damage done (int) in order to add that to the score
		//	CurrentScore += ScoreValue;
		//}
		//else CurrentScore += scoreFromDamage;
	//}


	//GetController()->PlayerState->Score = CurrentScore; // needs storing. Could pass this to the server to validate

	//NOTE: It isn't safe to store score on the actor, as it could potentially be cheated.

}

bool APlayerPaperCharacter::RecieveDamage_Implementation(int DamageAmount) {


	Super::RecieveDamage_Implementation(DamageAmount);

	return true;
}

void APlayerPaperCharacter::MulticastPlayFireSound_Implementation() {
	if (FireSound != nullptr) {
		UGameplayStatics::PlaySound2D(this, FireSound);
		//UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
}


