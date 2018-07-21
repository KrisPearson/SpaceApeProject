// // Copyright 2018 Kristiam Pearson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Interfaces/SpriteCharacterInterface.h"
#include "BasePaperCharacter.generated.h"


UENUM(BlueprintType)
enum class EFaceDirection : uint8 {
	FD_Left UMETA(DisplayName = "Face Left"),
	FD_Right UMETA(DisplayName = "Face Right"),
	FD_Up UMETA(DisplayName = "Face Up"),
	FD_Down UMETA(DisplayName = "Face Down")
};


/**
 * 
 */
UCLASS()
class SPACEAPEPROJECT_API ABasePaperCharacter : public APaperCharacter, public ISpriteCharacterInterface
{
	GENERATED_BODY()


protected:


	ABasePaperCharacter();


	virtual void BeginPlay() override;


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


	UPROPERTY(Replicated)
		EFaceDirection CurrentShootingDirection; // TODO: Move to state machine

	bool bIsShooting = false;

	UPROPERTY(Replicated)
		int CurrentHealthPoints;


	class UObjectPoolComponent* ProjectilePool;

	FVector FaceDirectionVector;

public:


	UPROPERTY(Replicated)
		EFaceDirection CurrentMovingDirection;// TODO: Move to state machine



	inline bool GetIsShooting() { return bIsShooting; }


	inline EFaceDirection GetCurrentShootingDirection() { return CurrentShootingDirection; }

	inline EFaceDirection GetCurrentMovingDirection() { return CurrentMovingDirection; }


	UFUNCTION()
	virtual void DealDamage(class AActor* ActorToDamage, int DamageAmount);

#pragma region CharacterInterface Methods 

	//ISpriteCharacterInterface method
	virtual FVector GetCharacterFaceDirection_Implementation() const override;


	//virtual bool DealDamage_Implementation(class ABasePaperCharacter* CharacterToDamage, int _DamageAmount);

	virtual bool RecieveDamage_Implementation(int DamageAmount) override;

#pragma endregion

private:






	//UPROPERTY(ReplicatedUsing = OnRep_ReplicatedShootingDirection)
	//EFaceDirection ReplicatedShootingDirection; // TODO: Move to state machine

	
};
