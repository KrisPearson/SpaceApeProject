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

UENUM()
enum class Heading {
	H_East= 0,
	H_North = 1,
	H_West = 2,
	H_South = 3,

	H_None = 99
};

UENUM(BlueprintType)
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
	class USpringArmComponent* CameraBoomComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shadow, meta = (AllowPrivateAccess = "true"))
	class UPaperCharacterAnimationComponent* AnimationComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shadow, meta = (AllowPrivateAccess = "true"))
	class USpriteShadowComponent* ShadowComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = "true"))
		class USoundBase* FireSound;



	virtual void Tick(float DeltaTime) override;

	// Static names for axis bindings
	static const FName MoveUpBinding;
	static const FName MoveRightBinding;
	static const FName FireUpBinding;
	static const FName FireRightBinding;

private:

	bool bCanFire = true;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;


	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface


	UPROPERTY()
	class UBaseWeaponComponent* EquippedWeaponComponent;

	struct FWeaponData* EquippedWeaponData;

	UFUNCTION(NetMulticast, Reliable)
		void MulticastPlayFireSound();
	virtual void MulticastPlayFireSound_Implementation();


	void UpdateCameraBounds(float DeltaTime);

	FVector CameraBoundsMin;
	FVector CameraBoundsMax;

	class UBoxComponent* CameraBoundsRef;



//public:
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		//PlayerMovementComponent* MovementComponent;

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	virtual void BeginPlay() override;

	void HandleShooting();

	void HandleMovement(float DeltaTime);



	EFaceDirection LastUpdatedMovingDirection;

	UPROPERTY(Replicated)
	EFaceDirection CurrentMovingDirection;


	UPROPERTY(Replicated)
	EFaceDirection CurrentShootingDirection;

	//UPROPERTY(ReplicatedUsing = OnRep_ReplicatedShootingDirection)
	EFaceDirection ReplicatedShootingDirection;



	UFUNCTION()
	void OnRep_ReplicatedShootingDirection();


	UFUNCTION(Server, Reliable, WithValidation)
	void ServerModifyMoveDirection(APlayerPaperCharacter* TargetedActor, EFaceDirection NewValue);
	virtual bool ServerModifyMoveDirection_Validate(APlayerPaperCharacter* TargetedActor, EFaceDirection NewValue) { return true; };
	virtual void ServerModifyMoveDirection_Implementation(APlayerPaperCharacter* TargetedActor, EFaceDirection NewValue) { TargetedActor->CurrentMovingDirection = NewValue; };


	// The default weapon, as assigned in the character blueprint. 
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UBaseWeaponComponent> DefaultWeaponComponent;




	bool bIsShooting = false;

	//The object count assigned to the projectile pool on its creation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shooting, meta = (AllowPrivateAccess = "true"))
		int DefaultProjectilePoolSize = 60;

	class UObjectPoolComponent* ProjectilePool;



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
		void DealDamage(AActor* _Enemy, int _DamageAmount);




public:
	APlayerPaperCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return ObliqueViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoomComponent; }

	inline bool GetIsShooting() { return bIsShooting; }

	inline EFaceDirection GetCurrentShootingDirection() { return CurrentShootingDirection; }

	inline EFaceDirection GetCurrentMovingDirection() { return CurrentMovingDirection; }

	void ChangeWeapon(TSubclassOf<class UBaseWeaponComponent> _NewWeapon);

	void SetCameraBounds(UBoxComponent* CameraBoundsBox);
};
