// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpriteObjects/BasePaperCharacter.h"
#include "PlayerPaperCharacter.generated.h"

//enum class EHeading :uint8 {
//	H_East UMETA(DisplayName = "East Heading") = 0,
//	H_North UMETA(DisplayName = "North Heading") = 1,
//	H_West UMETA(DisplayName = "West Heading") = 2,
//	H_South UMETA(DisplayName = "South Heading") = 3,
//
//	H_None UMETA(DisplayName = "No Heading") = -1
//};

UENUM()
enum class Heading {
	H_East= 0,
	H_North = 1,
	H_West = 2,
	H_South = 3,

	H_None = 99
};


//#include "Components/PlayerMovementComponent.h"
//#include "Components/PlayerMovementReplicator.h"

/**
 * 
 */
UCLASS()
class SPACEAPEPROJECT_API APlayerPaperCharacter : public ABasePaperCharacter
{
	GENERATED_BODY()


	


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shadow, meta = (AllowPrivateAccess = "true"))
		class UPlayerCameraControllerComponent* CameraController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoomComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* ObliqueViewCameraComponent;



	virtual void Tick(float DeltaTime) override;

	// Static names for axis bindings
	static const FName MoveUpBinding;
	static const FName MoveRightBinding;
	static const FName FireUpBinding;
	static const FName FireRightBinding;

private:

	bool bCanFire = true;

	/** Handle for management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired; // TODO: Move to weapon component ?

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface



	UFUNCTION(NetMulticast, Reliable)
		void MulticastPlayFireSound();
	virtual void MulticastPlayFireSound_Implementation(); //TODO: Consider best approach to handle audio channels



//public:
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		//PlayerMovementComponent* MovementComponent;

protected:

	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //TODO: TEMP


	virtual void BeginPlay() override;

	virtual void HandleShooting() override;

	virtual void HandleMovement(float DeltaTime) override;



	EFaceDirection LastUpdatedMovingDirection;



	UFUNCTION()
	void OnRep_ReplicatedShootingDirection();


	UFUNCTION(Server, Reliable, WithValidation)
	void ServerModifyMoveDirection(ABasePaperCharacter* TargetedActor, EFaceDirection NewValue);
	virtual bool ServerModifyMoveDirection_Validate(ABasePaperCharacter* TargetedActor, EFaceDirection NewValue) { return true; };
	virtual void ServerModifyMoveDirection_Implementation(ABasePaperCharacter* TargetedActor, EFaceDirection NewValue) { TargetedActor->CurrentMovingDirection = NewValue; };










	//TODO:Move to animation component

	void MoveUp(float Value);
	void MoveRight(float Value);
	void ShootUp(float Value);
	void ShootRight(float Value);

	int CurrentHorizontalShootValue = 0;
	int CurrentVerticalShootValue = 0;

	float CurrentHorizontalMoveValue = 0;
	float CurrentVerticalMoveValue = 0;

	void ShootToHeading(Heading HeadingDirection);

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerShootToHeading(Heading HeadingDirection);
	void ServerShootToHeading_Implementation(Heading HeadingDirection);
	bool ServerShootToHeading_Validate(Heading HeadingDirection) { return true;  };

	void ShotTimerExpired();

	UFUNCTION()
		virtual void DealDamage( AActor* ActorToDamage, int DamageAmount) override;


	virtual bool RecieveDamage_Implementation(int DamageAmount) override;



public:
	APlayerPaperCharacter();





};
