// // Copyright 2018 Kristiam Pearson. All Rights Reserved.

#include "BaseSpawnComponent.h"
#include "SpriteObjects/EnemyPaperCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "WorldObjects/BaseRoom.h"
#include "Components/BillboardComponent.h"


// Sets default values for this component's properties
UBaseSpawnComponent::UBaseSpawnComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Structure to hold one-time initialization
	// Structure to hold one-time initialization
	struct FConstructorStatics {
		// A helper class object we use to find target UTexture2D object in resource package
		ConstructorHelpers::FObjectFinderOptional<UTexture2D> MarkerTextureObject;

		// Icon sprite category name
		FName ID_CatagoryName;

		// Icon sprite display name
		FText NAME_DisplayName;

		FConstructorStatics()
			// Use helper class object to find the texture
			// "/Engine/EditorResources/S_Note" is resource path
			: MarkerTextureObject(TEXT("/Game/Icons/devil-512"))
			, ID_CatagoryName(TEXT("Marker"))
			, NAME_DisplayName(NSLOCTEXT("SpriteCategory", "AmmoMarker", "AmmoMarker")) {
		}
	};
	static FConstructorStatics ConstructorStatics;

	//SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(this, TEXT("SceneComp"));
	//RootComponent = SceneComponent;
	//RootComponent->Mobility = EComponentMobility::Static;

	BillboardComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("SceneComp"), true);
	BillboardComponent->Sprite = ConstructorStatics.MarkerTextureObject.Get();
	BillboardComponent->SetupAttachment(this);
}


// Called when the game starts
void UBaseSpawnComponent::BeginPlay()
{
	Super::BeginPlay();
	OwningRoom = Cast<ABaseRoom>(GetOwner());
	if (OwningRoom != nullptr) {
		OwningRoom->OnCharacterEnterDelegate.AddDynamic(this, &UBaseSpawnComponent::RegisterCharacterEnterRoom);
		OwningRoom->OnCharacterExitDelegate.AddDynamic(this, &UBaseSpawnComponent::RegisterCharacterExitRoom);
	}
}

// Called every frame
void UBaseSpawnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBaseSpawnComponent::HandleSpawnBehaviour(bool bEndBehaviour) {

	if (!bEndBehaviour) { // Start Spawning
		switch (SpawnBehaviour) {
		case ESpawnBehaviour::SB_OneTime:
			SpawnCharacter(DefaultSpawnType);
			SetSpawningDisabled(true);
			break;
		case ESpawnBehaviour::SB_EveryTime:
			SpawnCharacter(DefaultSpawnType);
			break;
		case ESpawnBehaviour::SB_Continuous:
			// Start Spawn Loop Timer
			if (SpawnedCharactersArray.Num() < MaxEnemyCount) SpawnCharacter(DefaultSpawnType);
			break;

		}
	}
	else { // End Spawning
		switch (SpawnBehaviour) {
		case ESpawnBehaviour::SB_OneTime:
			//SpawnCharacter(DefaultSpawnType);
			//SetSpawningDisabled(true);
			break;
		case ESpawnBehaviour::SB_EveryTime:
			DestroySpawnedCharacters();
			break;
		case ESpawnBehaviour::SB_Continuous:
			// End Spawn Loop Timer
			DestroySpawnedCharacters();
			break;

		}
	}
}

/* Performs the spawn event */
void UBaseSpawnComponent::SpawnCharacter(TSubclassOf<ABasePaperCharacter> TypeToSpawn) {
	if (!bIsDisabled) {

		if (GetOwnerRole() == ROLE_Authority) {

			ABasePaperCharacter* NewCharacter = GetWorld()->SpawnActor<ABasePaperCharacter>(TypeToSpawn,
				this->GetComponentLocation(),
				FRotator(0, 0, 0)
				//	SpawnParams
				);

			if (NewCharacter) {
				/// Tell the spawning zone that an enemy is spawning, so that the zone can provide  visual feeback to the player(s).
				//InformTargetZoneOfSpawn(Action.SpawnLocationData.ZoneToSpawnIn);

				// Assign a delegate, so that the SpawnComponent is informed when an enemy dies. 
				NewCharacter->CharacterDeathDelegate.AddDynamic(this, &UBaseSpawnComponent::RegisterCharacterDeath);
				SpawnedCharactersArray.Add(NewCharacter); // Add the character to the array

			}
		}
		else UE_LOG(LogTemp, Warning, TEXT("Role not authority"));
	}
}

void UBaseSpawnComponent::DestroySpawnedCharacters() {
	for (ABasePaperCharacter* Character : SpawnedCharactersArray) {
		Character->Destroy();
	}
}

#pragma region Delegate Methods

void UBaseSpawnComponent::RegisterCharacterDeath(ABasePaperCharacter* DeadCharacterRef) {
	SpawnedCharactersArray.Remove(DeadCharacterRef);

}

void UBaseSpawnComponent::RegisterCharacterEnterRoom(ABasePaperCharacter * EnteringCharacter, int32 NumberOfPlayersInRoom) {
	if (NumberOfPlayersInRoom > 0) {
		HandleSpawnBehaviour(false);                
	}
}

void UBaseSpawnComponent::RegisterCharacterExitRoom(ABasePaperCharacter * EnteringCharacter, int32 NumberOfPlayersInRoom) {
	if (NumberOfPlayersInRoom == 0) {
		HandleSpawnBehaviour(true);
	}
}
#pragma endregion
