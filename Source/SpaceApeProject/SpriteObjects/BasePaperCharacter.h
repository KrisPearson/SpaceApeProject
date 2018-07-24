// // Copyright 2018 Kristiam Pearson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Interfaces/SpriteObjectInterface.h"
#include "BasePaperCharacter.generated.h"


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
class SPACEAPEPROJECT_API ABasePaperCharacter : public APaperCharacter, public ISpriteObjectInterface
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

	//The object count assigned to the projectile pool on its creation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shooting, meta = (AllowPrivateAccess = "true"))
		int DefaultProjectilePoolSize = 30;

	// The default weapon, as assigned in the character blueprint. 
	UPROPERTY(Category = Character, EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class UBaseWeaponComponent> DefaultWeaponComponent;

	// The default health points attributed to the enemy
	UPROPERTY(EditAnywhere, Category = Stats, meta = (ClampMin = "10", ClampMax = "1000"))
		int HealthPoints = 10;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = "true"))
		class USoundBase* FireSound; // TODO: Move to AudioHandler
	
	


	UPROPERTY()
		class UBaseWeaponComponent* EquippedWeaponComponent;

	struct FWeaponData* EquippedWeaponData;

	void ChangeWeapon(TSubclassOf<class UBaseWeaponComponent> _NewWeapon);

	virtual void HandleShooting() { check(0 && "HandleShooting method requires override  method")};

	virtual void HandleMovement(float DeltaTime) { check(0 && "HandleMovement method requires override  method") };

	void UpdateFaceDirection();


	class UObjectPoolComponent* ProjectilePool;


	UPROPERTY(Replicated)
		int CurrentHealthPoints;

	UPROPERTY(Replicated)
		FVector CurrentShootingDirection;

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


	//UFUNCTION(Server, Reliable, WithValidation)
	//	void ServerModifyMoveDirection(ABasePaperCharacter* TargetedActor, FVector NewValue);
	//virtual bool ServerModifyMoveDirection_Validate(ABasePaperCharacter* TargetedActor, FVector NewValue) { return true; };
	//virtual void ServerModifyMoveDirection_Implementation(ABasePaperCharacter* TargetedActor, FVector NewValue) { TargetedActor->CurrentMovingDirection = NewValue; };

	//FVector LastUpdatedMovingDirection;

	//UFUNCTION(Server, Reliable, WithValidation)
	//	void ServerModifyIsShooting(ABasePaperCharacter* TargetedActor, bool NewValue);
	//virtual bool ServerModifyIsShooting_Validate(ABasePaperCharacter* TargetedActor, bool NewValue) { return true; };
	//virtual void ServerModifyIsShooting_Implementation(ABasePaperCharacter* TargetedActor, bool NewValue) { TargetedActor->bIsShooting = NewValue; };


	FVector FaceDirectionVector;

public:


	inline bool GetIsShooting() { return bIsShooting; }


	inline FVector GetCurrentShootingDirection() { return CurrentShootingDirection; }

	//inline EFaceDirection GetCurrentMovingDirection() { return CurrentMovingDirection; }


	UFUNCTION()
	virtual void DealDamage(class AActor* ActorToDamage, int DamageAmount);

#pragma region Interface Methods 

	virtual FVector GetObjectFaceDirection_Implementation() const override;

	virtual bool RecieveDamage_Implementation(int DamageAmount) override;

#pragma endregion

};
