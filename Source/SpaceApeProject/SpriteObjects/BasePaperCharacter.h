// // Copyright 2018 Kristian Pearson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Interfaces/SpriteObjectInterface.h"
#include "Interfaces/DamageableInterface.h"
#include "Classes/GenericTeamAgentInterface.h"
#include "Enums/TeamOwnerEnum.h"
#include "BasePaperCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDeath, ABasePaperCharacter*, CharacterReference);

UENUM(BlueprintType)
enum class EFaceDirection : uint8 {
	FD_Left UMETA(DisplayName = "Face Left"),
	FD_Right UMETA(DisplayName = "Face Right"),
	FD_Up UMETA(DisplayName = "Face Up"),
	FD_Down UMETA(DisplayName = "Face Down")
};


extern float CameraAngle;

namespace SpriteDirection {
	extern FVector Up;
	extern FVector Down;
	extern FVector Right;
	extern FVector Left;
}


UCLASS()
class SPACEAPEPROJECT_API ABasePaperCharacter : public APaperCharacter, public ISpriteObjectInterface, public IDamageableInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()


protected:


	ABasePaperCharacter();


	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
		class UWorld* World;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sprite, meta = (AllowPrivateAccess = "true"))
	class UPaperCharacterAnimationComponent* AnimationComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shadow, meta = (AllowPrivateAccess = "true"))
	class USpriteShadowComponent* ShadowComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shadow, meta = (AllowPrivateAccess = "true"))
	class UCollissionDamageComponent*  CollissionDamageComponent;

	//The object count assigned to the projectile pool on its creation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shooting, meta = (AllowPrivateAccess = "true"))
		int DefaultProjectilePoolSize = 30;

	// The default weapon, as assigned in the character blueprint. 
	UPROPERTY(Category = Character, EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class UBaseWeaponComponent> DefaultWeaponComponent;

	// The default health points attributed to the enemy
	UPROPERTY(EditAnywhere, Category = Stats, meta = (ClampMin = "10", ClampMax = "1000"))
		int HealthPoints = 100;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* PivotComponent;

	// Used to prevent multiple hit events firing on collission
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollissionOverlapComponent;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = "true"))
		class USoundBase* FireSound; // TODO: Move to AudioHandler (?)
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = "true"))
		class USoundBase* HitSound; // TODO: Move to AudioHandler (?)


	UPROPERTY()
		class UBaseWeaponComponent* EquippedWeaponComponent;

	struct FWeaponData* EquippedWeaponData;

	void ChangeWeapon(TSubclassOf<class UBaseWeaponComponent> _NewWeapon);

	virtual void HandleShooting();

	virtual void HandleMovement(float DeltaTime) { check(0 && "HandleMovement method requires override  method") };

	void UpdateFaceDirection();


	class UObjectPoolComponent* ProjectilePool;


	UPROPERTY(Replicated)
		int CurrentHealthPoints;

	UPROPERTY(Replicated)
		FVector CurrentShootingDirection;


	UFUNCTION(Reliable, Server, WithValidation)
		void ServerSetShootAxisValues(int HorizontalShootValue, int VerticalShootValue);
	bool ServerSetShootAxisValues_Validate(int HorizontalShootValue, int VerticalShootValue) { return true; };
	void ServerSetShootAxisValues_Implementation(int HorizontalShootValue, int VerticalShootValue) { CurrentHorizontalShootValue = HorizontalShootValue; CurrentVerticalShootValue = VerticalShootValue; };


	UFUNCTION(BlueprintCallable)
		void ShootInDirection(FVector DirectionToShoot);

	UFUNCTION(Reliable, Server, WithValidation)
		void ServerSetCurrentShootingDirection(FVector NewDirection);
	bool ServerSetCurrentShootingDirection_Validate(FVector NewDirection) { return true;  };
	void ServerSetCurrentShootingDirection_Implementation(FVector NewDirection) { CurrentShootingDirection = NewDirection;  };



	UPROPERTY(Replicated)
		FVector CurrentMovingDirection;

	int CurrentHorizontalShootValue = 0;
	int CurrentVerticalShootValue = 0;

	void UpdateIsShooting();

	UPROPERTY(Replicated)
	bool bIsShooting;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetIsShooting(bool newValue);
	bool ServerSetIsShooting_Validate(bool NewValue) { return true; };
	void ServerSetIsShooting_Implementation(bool NewValue) { 
		if (NewValue) {
			UE_LOG(LogTemp, Warning, TEXT("ServerSetIsShooting_Implementation == true"));
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("ServerSetIsShooting_Implementation == false"));
		}
			bIsShooting = NewValue; 
	};

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetIsShooting(bool NewValue);
	void MulticastSetIsShooting_Implementation(bool NewValue) { bIsShooting = NewValue;  };

	FVector FaceDirectionVector;

	// Used to reinstate movement input following override
	FTimerHandle MovementOverrideTimer;
	bool bInputDisabled = false;


	// Used when forcing a character to move in a direction
	FVector OverrideDirection;

	UFUNCTION()
		void EnableCharacterInput();

public:

	const class ABaseRoom* GetCurrentRoom() { return CurrentRoom;  }

	void SetCurrentRoom(class ABaseRoom* NewRoom) { CurrentRoom = NewRoom; }

	UFUNCTION(BlueprintCallable)
		void SetShootAxisValues(int HorizontalShootValue, int VerticalShootValue);


	void ForceMoveToLocation(FVector TargetLocation);



	inline bool GetIsShooting() { return bIsShooting; }


	inline FVector GetCurrentShootingDirection() { return CurrentShootingDirection; }

#pragma region Interface Methods 

	virtual FVector GetObjectFaceDirection_Implementation() const override;

	virtual bool ReceiveDamage_Implementation(float DamageAmount, AActor* DamageInstigator, FGenericTeamId DamageFromTeam) override;

	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;

	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override;

#pragma endregion

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnCharacterDeath CharacterDeathDelegate;

	void SetCurrentRoomBounds(const class UBoxComponent & RoomBoundsBox);

	UFUNCTION(BlueprintCallable, Category = "Room" )
		FVector	GetRoomMinBounds() { return MinMaxRoomBounds.Key;  };

	UFUNCTION(BlueprintCallable, Category = "Room" )
		FVector GetRoomMaxBounds() { return MinMaxRoomBounds.Value; };

	//UFUNCTION(BlueprintCallable, Category = "Room")
		//TPair<FVector, FVector> GetRoomBounds() { return MinMaxRoomBounds; };


protected:

	FGenericTeamId TeamId;

	//TeamOwner::ETeamOwner TeamOwner;

	TPair<FVector, FVector> MinMaxRoomBounds;

	FVector MinRoomBounds;
	FVector MaxRoomBounds;

	class UBoxComponent* BoundingBoxRef;

	class UMaterialInstanceDynamic* DynamicSpriteMaterial;

	UFUNCTION(NetMulticast, reliable)
		void MulticastPlayDamageFlash();
	void MulticastPlayDamageFlash_Implementation();


	UFUNCTION()
		void CharacterDeath();

		bool CheckIfAlive();

		//UFUNCTION(BlueprintCallable)
		//	void ShootInDirection(EFaceDirection DirectionToShoot);

	private:

		class ABaseRoom* CurrentRoom;

		void ShootWeapon();

		UFUNCTION(Reliable, Server, WithValidation)
			void ServerShootWeapon();
		void ServerShootWeapon_Implementation();
		bool ServerShootWeapon_Validate() { return true; };




		

		bool bMovementControlOverridden;

};
