// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpriteObjects/BasePaperCharacter.h"
#include "EnemyPaperCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeath, AEnemyPaperCharacter*, EnemyPointer);

/**
 * 
 */
UCLASS()
class SPACEAPEPROJECT_API AEnemyPaperCharacter : public ABasePaperCharacter 
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;

	AEnemyPaperCharacter();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// The maximum movement speed of the enemy
	UPROPERTY(EditAnywhere, Category = Stats, meta = (ClampMin = "10", ClampMax = "1000"))
		float MaxSpeed = 500;

	// The Value awarded to the player upon death
	UPROPERTY(EditAnywhere, Category = Stats, meta = (ClampMin = "0", ClampMax = "9999", UIMin = "0", UIMax = "9999"))
		int ScoreValue = 100;

	// The liklihood of this class spawning [Used in Random enemy wave spawning]
	UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "0", ClampMax = "10", UIMin = "0", UIMax = "10"))
		float SpawnProbability = 5;

	// Tho cost to spawn the class [Used in Random enemy wave spawning]
	UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "100", ClampMax = "10000", UIMin = "100", UIMax = "10000"))
		int SpawnCost = 1;


	UPROPERTY(EditAnywhere, Category = Behaviour)
		class UBehaviorTree* BotBehavior;


	//UFUNCTION()
	//	void DisableMaterialFlash();

	UFUNCTION()
		inline int GetScoreValue() { return ScoreValue; }

	UFUNCTION()
		inline float GetSpawnProbability() { return SpawnProbability; }

	UFUNCTION()
		inline int GetSpawnCost() { return SpawnCost;  };

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnEnemyDeath EnemyDeathDelegate;

public:
	
	UFUNCTION()
		virtual void ReceiveDamage(int _DamageAmount, bool& _IsDead, int& _ScoreToAdd);
private:



	UWorld* World;
	
	class UMaterialInstanceDynamic* DynamicEnemyMaterial;

	//FTimerHandle DamageFlashTimerHandle;

	//UPROPERTY(EditAnywhere, Category = Behaviour)
		//class UBehaviorTree* BotBehavior;
	
	bool CheckIfAlive();

protected:

	UFUNCTION(NetMulticast, reliable)
		void MulticastPlayDamageFlash();
	void MulticastPlayDamageFlash_Implementation();



	UFUNCTION()
		void EnemyDeath();



	virtual bool RecieveDamage_Implementation(int DamageAmount) override;
};
