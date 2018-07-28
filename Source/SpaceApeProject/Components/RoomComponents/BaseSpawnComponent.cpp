// // Copyright 2018 Kristiam Pearson. All Rights Reserved.

#include "BaseSpawnComponent.h"
#include "SpriteObjects/EnemyPaperCharacter.h"
#include "UObject/ConstructorHelpers.h"
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

	// We need a scene component to attach Icon sprite


	//SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(this, TEXT("SceneComp"));
	//RootComponent = SceneComponent;
	//RootComponent->Mobility = EComponentMobility::Static;


	BillboardComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("SceneComp"), true);

	//static FConstructorStatics ConstructorStatics;
	BillboardComponent->Sprite = ConstructorStatics.MarkerTextureObject.Get(); ;

	BillboardComponent->AttachTo(this);
	// ...
}


// Called when the game starts
void UBaseSpawnComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBaseSpawnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

/* Performs the spawn event */
void UBaseSpawnComponent::SpawnCharacter(TSubclassOf<ABasePaperCharacter> TypeToSpawn) {
	UE_LOG(LogTemp, Warning, TEXT("UBaseSpawnComponent::SpawnEnemy"));

	if (GetOwnerRole() == ROLE_Authority) {

		ABasePaperCharacter* NewCharacter = GetWorld()->SpawnActor<ABasePaperCharacter>(TypeToSpawn,
		this->GetComponentLocation(),
		FRotator(0, 0, 0) // this could be set to face inwards, perhaps using the spawn zone's rotation
		//	SpawnParams
			);

		if (NewCharacter) {
			/// Tell the spawning zone that an enemy is spawning, so that the zone can provide  visual feeback to the player(s).
			//InformTargetZoneOfSpawn(Action.SpawnLocationData.ZoneToSpawnIn);

			/// Assign a delegate, so that the Wave Manager is informed when an enemy dies. 
			NewCharacter->CharacterDeathDelegate.AddDynamic(this, &UBaseSpawnComponent::RegisterCharacterDeath);
			SpawnedCharactersArray.Add(NewCharacter); // Add the character to the array
			// This could potentially be improved, as currently, if  bug or other unforeseen event causes the enemy to be unkillable by
			// the player, or this delegate fails to inform the manager of its death, then no new waves will spawn.

		}
	}
	else UE_LOG(LogTemp, Warning, TEXT("Role not authority"));
}


void UBaseSpawnComponent::RegisterCharacterDeath(ABasePaperCharacter* DeadCharacterRef) {
	SpawnedCharactersArray.Remove(DeadCharacterRef);

}
