// // Copyright 2018 Kristiam Pearson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseSpawn.generated.h"


UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ESpawnBehaviour : uint8 {
	SB_OnRequest UMETA(DisplayName = "OnRequest"), // Will not automatically spawn characters, but instead requires the spawn function to be called.
	SB_OneTime UMETA(DisplayName = "OneTimeOnly"), // Will only spawn a single character once during a playthrough (useful for boss-like enemies)
	SB_EveryTime UMETA(DisplayName = "EveryTime"), // Spawns a character each time a room is entered, then destroys them on exit
	SB_Continuous UMETA(DisplayName = "Continuous") // Continuously spawns characters until a predifined limit is met whilst the player is in the room
};

class ABasePaperCharacter;

UCLASS()
class SPACEAPEPROJECT_API ABaseSpawn : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseSpawn();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawning")
		bool IsPlayerSpawn = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawning")
		TSubclassOf<ABasePaperCharacter> DefaultSpawnType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawning")
		ESpawnBehaviour SpawnBehaviour;

	// The maximum number of enemies spawned by this spawner to be simultainuoisly alive
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawning")
		int MaxEnemyCount = 4;

protected:

	virtual void Tick(float DeltaTime) override;

	// Called when the game starts
	virtual void BeginPlay() override;

	//Disabling the SpawnComponent will prevent any spawn behaviour from firing.
	bool bIsDisabled = false;

	TArray<ABasePaperCharacter*> SpawnedCharactersArray;


	virtual void HandleSpawnBehaviour(bool bEndBehaviour);

	// virtual void HandleVisuals(); 

public:
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable, Category = "Spawning")
		void SpawnCharacter(TSubclassOf<ABasePaperCharacter> TypeToSpawn);

	UFUNCTION(BlueprintCallable, Category = "Spawning")
		void DestroySpawnedCharacters();

	class UBillboardComponent* BillboardComponent;


	UFUNCTION(BlueprintCallable, Category = "Spawning")
		void SetSpawningDisabled(bool bIsDisabled) {
		this->bIsDisabled = bIsDisabled;
	};


	void SpawnPlayerCharacter(class AController* NewPlayerController);



private:

	UFUNCTION()
		void RegisterCharacterDeath(ABasePaperCharacter* DeadCharacterRef);
	// TODO: Bind event to enemy death. Call blueprint event too.

	UFUNCTION()
		void RegisterCharacterEnterRoom(ABasePaperCharacter* EnteringCharacter, int32 NumberOfPlayersInRoom);

	UFUNCTION()
		void RegisterCharacterExitRoom(ABasePaperCharacter* EnteringCharacter, int32 NumberOfPlayersInRoom);

	class ABaseRoom* OwningRoom;
};