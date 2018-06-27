// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "PlayerPaperCharacter.generated.h"

//enum class EHeading :uint8 {
//	H_East UMETA(DisplayName = "East Heading") = 0,
//	H_North UMETA(DisplayName = "North Heading") = 1,
//	H_West UMETA(DisplayName = "West Heading") = 2,
//	H_South UMETA(DisplayName = "South Heading") = 3,
//
//	H_None UMETA(DisplayName = "No Heading") = -1
//};

enum class Heading  {
	H_East= 0,
	H_North = 1,
	H_West = 2,
	H_South = 3,

	H_None = -1
};


enum class EFaceDirection : uint8 {
	FD_Left UMETA(DisplayName = "Face Left"),
	FD_Right UMETA(DisplayName = "Face Right"),
	FD_Up UMETA(DisplayName = "Face Up"),
	FD_Down UMETA(DisplayName = "Face Down")
};

//#include "Components/PlayerMovementComponent.h"
//#include "Components/PlayerMovementReplicator.h"

/**
 * 
 */
UCLASS()
class SPACEAPEPROJECT_API APlayerPaperCharacter : public APaperCharacter
{
	GENERATED_BODY()

		class UWorld* World;


	//TODO: Implement camera switching on the controller class (each room could hold)
	/** Oblique view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class UCameraComponent* ObliqueViewCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Projects a shadow beneath the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* ShadowDecal;


	virtual void Tick(float DeltaSeconds) override;

	// Static names for axis bindings
	static const FName MoveUpBinding;
	static const FName MoveRightBinding;
	static const FName FireUpBinding;
	static const FName FireRightBinding;

private:

	bool bCanFire = true;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;

	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shadow, meta = (AllowPrivateAccess = "true"))
		class USpriteShadowComponent* ShadowComponent;

//public:
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		//PlayerMovementComponent* MovementComponent;

protected:

	virtual void BeginPlay() override;

	EFaceDirection CurrentMovingDirection;
	EFaceDirection CurrentShootingDirection;
	//bool bIsMoving = false;
	bool bIsShooting = false;

	//TODO:Move to animation component

	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* WalkRightFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* WalkLeftFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* WalkUpFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* WalkDownFlipbook;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* IdleRightFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* IdleLeftFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* IdleUpFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* IdleDownFlipbook;

	

	void MoveUp(float Value);
	void MoveRight(float Value);
	void ShootUp(float Value);
	void ShootRight(float Value);


	int CurrentHorizontalShootValue = 0;
	int CurrentVerticalShootValue = 0;

	void ShootToHeading(Heading HeadingDirection);

	void ShotTimerExpired();

	UFUNCTION()
		void DealDamage(AActor* _Enemy, int _DamageAmount);

public:
	APlayerPaperCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return ObliqueViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};
