// // Copyright 2018 Kristiam Pearson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BaseSpawnComponent.generated.h"


UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ESpawnBehaviour : uint8 {
	SB_OneTime UMETA(DisplayName = "OneTimeOnly"), // Will only spawn a single character once during a playthrough (useful for bosses)
	SB_EveryTime UMETA(DisplayName = "EveryTime"), // Spawns a character each time a room is entered, then destroys them on exit
	SB_Continuous UMETA(DisplayName = "Continuous") // Continuously spawns characters until a predifined limit is met whilst the player is in the room
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEAPEPROJECT_API UBaseSpawnComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBaseSpawnComponent();

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawning")
		//TSubclassOf<ABasePaperCharacter> TypeToSpawn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawning")
		ESpawnBehaviour SpawnBehaviour;

	// The maximum number of enemies spawned by this spawner to be simultainuoisly alive
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawning")
		int MaxEnemyCount = 4;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//Disabling the SpawnComponent will prevent any spawn behaviour from firing.
	bool bIsDisabled = false;

	TArray<ABasePaperCharacter*> SpawnedCharactersArray;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	UFUNCTION(BlueprintCallable, Category = "Spawning")
		void SpawnCharacter(TSubclassOf<ABasePaperCharacter> TypeToSpawn);

	class UBillboardComponent* BillboardComponent;


	UFUNCTION(BlueprintCallable, Category = "Spawning")
		void SetIsDisables(bool bIsDisabled) { this->bIsDisabled = bIsDisabled; };



private:

	void RegisterCharacterDeath(ABasePaperCharacter* DeadCharacterRef);
	// TODO: Bind event to enemy death. Call blueprint event too.
};
