// Fill out your copyright notice in the Description page of Project Settings.

/*
PaperCharacterAnimationComponent.cpp
Purpose: Intended for use on all sprite characters - it stores and applies animations to sprites
based upon the state of the component's owning Character.

@author Kristian Pearson
@version 0.5 17/18/2018
*/

#include "PaperCharacterAnimationComponent.h"
#include "PaperFlipbookComponent.h"


#include "SpriteObjects/BasePaperCharacter.h" // TODO: Use a base class shared by all paper characters


// Sets default values for this component's properties
UPaperCharacterAnimationComponent::UPaperCharacterAnimationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPaperCharacterAnimationComponent::BeginPlay()
{
	Super::BeginPlay();


	OwningCharacter = Cast<ABasePaperCharacter>(GetOwner());

	
}


// Called every frame
void UPaperCharacterAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateAnimation();

	// ...
}

/* Assigns an animation from one of several predefined sprite animations to the character 
sprite based on movement and shooting conditions*/
void UPaperCharacterAnimationComponent::UpdateAnimation() {
	UPaperFlipbook* DesiredAnimation = nullptr;

	//const FVector PlayerVelocity = GetOwner()->GetVelocity();
	const bool bIsMoving = GetOwner()->GetVelocity().SizeSquared() > 0 ? true : false;

	//UE_LOG(LogTemp, Warning, TEXT("Player Velocity  =  %f , %f, %f"), PlayerVelocity.X, PlayerVelocity.Y, PlayerVelocity.Z);

	//TODO: Define a more appropriate state machine or see if one exists in unreal already for sprites

#pragma region Animation Conditions
	if (OwningCharacter != nullptr) {

		FVector FaceDirection;
		ISpriteObjectInterface* ObjectInterface = Cast<ISpriteObjectInterface>(OwningCharacter);
		if (ObjectInterface) FaceDirection = ISpriteObjectInterface::Execute_GetObjectFaceDirection(OwningCharacter);

		FVector ShootDirection = OwningCharacter->GetCurrentShootingDirection();

		if (OwningCharacter->GetIsShooting()) { // Shooting Animation
			//UE_LOG(LogTemp, Warning, TEXT("Shooting Animation %s"), *(OwningCharacter->GetFName().ToString()) );
			if (ShootDirection == SpriteDirection::Up) {
				if (bIsMoving) DesiredAnimation = WalkUpFlipbook;
				else DesiredAnimation = IdleUpFlipbook;
			}
			else if (ShootDirection == SpriteDirection::Down) {
				if (bIsMoving) DesiredAnimation = WalkDownFlipbook;
				else DesiredAnimation = IdleDownFlipbook;
			}
			else if (ShootDirection == SpriteDirection::Left) {
				if (bIsMoving) DesiredAnimation = WalkLeftFlipbook;
				else DesiredAnimation = IdleLeftFlipbook;
			}
			else if (ShootDirection == SpriteDirection::Right) {
				if (bIsMoving) DesiredAnimation = WalkRightFlipbook;
				else DesiredAnimation = IdleRightFlipbook;
			}
		}
		else if (bIsMoving) { // Walking Animation
			//if (GetOwner()->Role > ROLE_Authority)  UE_LOG(LogTemp, Warning, TEXT("Walking Animation %s"), *(OwningCharacter->GetFName().ToString()) );
			if (FaceDirection == SpriteDirection::Up) {
				DesiredAnimation = WalkUpFlipbook;
			}
			else if (FaceDirection == SpriteDirection::Down) {
				DesiredAnimation = WalkDownFlipbook;
			}
			else if (FaceDirection == SpriteDirection::Left) {
				DesiredAnimation = WalkLeftFlipbook;
			}
			else if (FaceDirection == SpriteDirection::Right) {
				DesiredAnimation = WalkRightFlipbook;
			}
		}
		else { // Idle animation
			//UE_LOG(LogTemp, Warning, TEXT("Idle animation %s"), *(OwningCharacter->GetFName().ToString()) );
			if (FaceDirection == SpriteDirection::Up) {
				DesiredAnimation = IdleUpFlipbook;
			}
			else if (FaceDirection == SpriteDirection::Down) {
				DesiredAnimation = IdleDownFlipbook;
			}
			else if (FaceDirection == SpriteDirection::Left) {
				DesiredAnimation = IdleLeftFlipbook;
			}
			else if (FaceDirection == SpriteDirection::Right) {
				DesiredAnimation = IdleRightFlipbook;
			}
		}


		//if (OwningCharacter->GetIsShooting()) { // Shooting Animation
		//	
		//	switch (OwningCharacter->GetCurrentShootingDirection()) {
		//	case EFaceDirection::FD_Left:
		//		if (bIsMoving) DesiredAnimation = WalkLeftFlipbook;
		//		else DesiredAnimation = IdleLeftFlipbook;
		//		break;
		//	case EFaceDirection::FD_Right:
		//		if (bIsMoving) DesiredAnimation = WalkRightFlipbook;
		//		else DesiredAnimation = IdleRightFlipbook;
		//		break;
		//	case EFaceDirection::FD_Up:
		//		if (bIsMoving) DesiredAnimation = WalkUpFlipbook;
		//		else DesiredAnimation = IdleUpFlipbook;
		//		break;
		//	case EFaceDirection::FD_Down:
		//		if (bIsMoving) DesiredAnimation = WalkDownFlipbook;
		//		else DesiredAnimation = IdleDownFlipbook;
		//		break;
		//	default:
		//		UE_LOG(LogTemp, Warning, TEXT("bIsShooting switch FAILED"));
		//		break;
		//	}
		//}
		//else if (bIsMoving) { //Walking Animation
		//	switch (OwningCharacter->GetCurrentMovingDirection()) {
		//	case EFaceDirection::FD_Left:
		//		DesiredAnimation = WalkLeftFlipbook;
		//		break;
		//	case EFaceDirection::FD_Right:
		//		DesiredAnimation = WalkRightFlipbook;
		//		break;
		//	case EFaceDirection::FD_Up:
		//		DesiredAnimation = WalkUpFlipbook;
		//		break;
		//	case EFaceDirection::FD_Down:
		//		DesiredAnimation = WalkDownFlipbook;
		//		break;
		//	default:
		//		UE_LOG(LogTemp, Warning, TEXT("bIsMoving switch FAILED"));
		//		break;
		//	}
		//}
		//else { // Idle animation
		//	switch (OwningCharacter->GetCurrentMovingDirection()) {
		//	case EFaceDirection::FD_Left:
		//		DesiredAnimation = IdleLeftFlipbook;
		//		break;
		//	case EFaceDirection::FD_Right:
		//		DesiredAnimation = IdleRightFlipbook;
		//		break;
		//	case EFaceDirection::FD_Up:
		//		DesiredAnimation = IdleUpFlipbook;
		//		break;
		//	case EFaceDirection::FD_Down:
		//		DesiredAnimation = IdleDownFlipbook;
		//		break;
		//	default:
		//		UE_LOG(LogTemp, Warning, TEXT("Idle switch FAILED"));
		//		break;
		//	}
		//}
#pragma endregion

		//UPaperFlipbook* DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? RunningAnimation : IdleAnimation;
		if (DesiredAnimation != nullptr && OwningCharacter->GetSprite()->GetFlipbook() != DesiredAnimation) {
			OwningCharacter->GetSprite()->SetFlipbook(DesiredAnimation);
		}
	}
}

