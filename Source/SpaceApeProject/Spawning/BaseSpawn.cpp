// // Copyright 2018 Kristian Pearson. All Rights Reserved.

#include "BaseSpawn.h"
#include "SpriteObjects/BasePaperCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "WorldObjects/BaseRoom.h"
#include "Components/BillboardComponent.h"
#include "GameFramework/Controller.h"


// Sets default values
ABaseSpawn::ABaseSpawn() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;

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

	//BillboardComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("SceneComp"), true);
	//BillboardComponent->Sprite = ConstructorStatics.MarkerTextureObject.Get();
	//BillboardComponent->SetupAttachment(this);
}

// Called when the game starts or when spawned
void ABaseSpawn::BeginPlay() {
	Super::BeginPlay();
	OwningRoom = Cast<ABaseRoom>(GetOwner());
	if (OwningRoom != nullptr) {
		OwningRoom->OnCharacterEnterDelegate.AddDynamic(this, &ABaseSpawn::RegisterCharacterEnterRoom);
		OwningRoom->OnCharacterExitDelegate.AddDynamic(this, &ABaseSpawn::RegisterCharacterExitRoom);
	}
}

// Called every frame
void ABaseSpawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void ABaseSpawn::HandleSpawnBehaviour(bool bEndBehaviour) {
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
void ABaseSpawn::SpawnCharacter(TSubclassOf<ABasePaperCharacter> TypeToSpawn) {
	if (!bIsDisabled) {

		if (Role == ROLE_Authority) {

			ABasePaperCharacter* NewCharacter = GetWorld()->SpawnActor<ABasePaperCharacter>(TypeToSpawn,
				this->GetActorLocation(),
				FRotator(0, 0, 0)
				//	SpawnParams
				);

			if (NewCharacter) {
				/// Tell the spawning zone that an enemy is spawning, so that the zone can provide  visual feeback to the player(s).
				//InformTargetZoneOfSpawn(Action.SpawnLocationData.ZoneToSpawnIn);

				// Assign a delegate, so that the SpawnComponent is informed when an enemy dies. 
				NewCharacter->CharacterDeathDelegate.AddDynamic(this, &ABaseSpawn::RegisterCharacterDeath);
				SpawnedCharactersArray.Add(NewCharacter); // Add the character to the array


				Cast<ABaseRoom>(GetOwner())->AddCharacterToRoomPop(NewCharacter);

			}
		}
		else UE_LOG(LogTemp, Warning, TEXT("Role not authority"));
	}
}

void ABaseSpawn::DestroySpawnedCharacters() {
	for (ABasePaperCharacter* Character : SpawnedCharactersArray) {
		Character->Destroy();
	}
}

#pragma region Delegate Methods

void ABaseSpawn::SpawnPlayerCharacter(AController * NewPlayerController) {
	if (Role == ROLE_Authority) {

		if (NewPlayerController) {



			// Assign a delegate, so that the SpawnComponent is informed when an enemy dies. 
			//NewCharacter->CharacterDeathDelegate.AddDynamic(this, &ABaseSpawn::RegisterCharacterDeath);
			//SpawnedCharactersArray.Add(NewCharacter); // Add the character to the array


			ABasePaperCharacter* PlayerCharacter = Cast<ABasePaperCharacter>(NewPlayerController->GetPawn());
			if (PlayerCharacter) Cast<ABaseRoom>(GetOwner())->AddCharacterToRoomPop(PlayerCharacter);

		}
	}
	else UE_LOG(LogTemp, Warning, TEXT("Role not authority"));
}

void ABaseSpawn::RegisterCharacterDeath(ABasePaperCharacter* DeadCharacterRef) {
	SpawnedCharactersArray.Remove(DeadCharacterRef);

}

void ABaseSpawn::RegisterCharacterEnterRoom(ABasePaperCharacter * EnteringCharacter, int32 NumberOfPlayersInRoom) {
	if (NumberOfPlayersInRoom > 0) {
		HandleSpawnBehaviour(false);
	}
}

void ABaseSpawn::RegisterCharacterExitRoom(ABasePaperCharacter * EnteringCharacter, int32 NumberOfPlayersInRoom) {
	if (NumberOfPlayersInRoom == 0) {
		HandleSpawnBehaviour(true);
	}
}
#pragma endregion
