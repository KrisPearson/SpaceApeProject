
/**
BaseRoom.cpp
Purpose: Contains a section of environment art as well as components involved
in gameplay events. 

The BaseRoom is intended to be inherrited by Blueprint objects, in which 
gameplay events can be scripted and sequenced. The Room will handle the initiation
and conclusion of said events.

@author Kristian Pearson
@version 1.0 17/18/2018
*/

#include "BaseRoom.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "WorldObjects/BaseDoor.h"
#include "GameFramework/SpringArmComponent.h"

#include "Components/PlayerCameraControllerComponent.h"

#include "SpriteObjects/PlayerPaperCharacter.h"

//const float CAMERA_ANGLE = -60.0f; // TODO: Make global
//extern float CameraAngle;

// Sets default values
ABaseRoom::ABaseRoom() {

	RoomMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Room Mesh"));

	CameraBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Camera Bounds Box"));
	CameraBounds->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CameraBounds->SetupAttachment(RoomMesh);

	RoomBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Room Bounds Box"));
	RoomBounds->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	RoomBounds->OnComponentBeginOverlap.AddDynamic(this, &ABaseRoom::OnComponentEnterRoom);
	RoomBounds->OnComponentEndOverlap.AddDynamic(this, &ABaseRoom::OnComponentExitRoom);
	RoomBounds->SetupAttachment(RoomMesh);

	DoorsToNeighbourRooms = TMap<ABaseDoor*, ABaseRoom*>();

	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ABaseRoom::BeginPlay() {
	Super::BeginPlay();

	// Get the dimensions of the room mesh and use it to 
	//define the trigger box and camera bounds for the room
	FVector RoomDimensions = RoomMesh->GetStaticMesh()->GetBounds().GetBox().GetSize();
	RoomDimensions = RoomDimensions / 2;
	if (RoomBounds != nullptr) RoomBounds->SetBoxExtent(RoomDimensions * 0.95f);
	if (CameraBounds !=  nullptr) CameraBounds->SetBoxExtent(RoomDimensions * 0.7f);

	TArray<AActor*> Children;
	GetAllChildActors(Children, false);
	for (AActor* Child : Children) { Child->SetOwner(this); }

	UpdateBoundsForRoomPop(); // Update bounds following box initialisation
}


// Called every frame
void ABaseRoom::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ABaseRoom::UpdateBoundsForRoomPop() {
	for (TPair<ABasePaperCharacter*, bool> Char : CharactersInRoom) {
		if (Char.Value) {
			auto* CameraController = Char.Key->GetComponentByClass(UPlayerCameraControllerComponent::StaticClass());
			if (CameraController) Cast<UPlayerCameraControllerComponent>(CameraController)->SetCameraBounds(*GetGameraBoundsBox());
		}
		Char.Key->SetCurrentRoomBounds(*GetRoomBoundsBox());
	}
}

/*The intended use of this method is to detect the player character entering the room, at which point the room will initiate its event sequence. */
void ABaseRoom::OnComponentEnterRoom(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) {
			
	//TODO: Restructure the enter and leave methods
	// - Discern if character > then type of character  > then what to do with each type

	if (OtherActor->IsA(ABasePaperCharacter::StaticClass())) {

		// Handles the first time a player enters a room
		ABasePaperCharacter* Character = Cast<ABasePaperCharacter>(OtherActor);
		if (Character->GetCurrentRoom() == nullptr) {
			AddCharacterToRoomPop(Character);
			UE_LOG(LogTemp, Warning, TEXT("Character->GetCurrentRoom() == nullptr is true"));
		}
		else 	UE_LOG(LogTemp, Warning, TEXT("Character->GetCurrentRoom() == nullptr is false"));

	//	if (APlayerPaperCharacter* Character = Cast<APlayerPaperCharacter>(OtherActor)) {

			//if (!PlayerCharactersInRoom.Contains(Character)) {

			//	PlayerCharactersInRoom.Add(Character);
			//	OnCharacterEnterDelegate.Broadcast(Cast<ABasePaperCharacter>(OtherActor), PlayerCharactersInRoom.Num());

			//	auto* CameraController = OtherActor->GetComponentByClass(UPlayerCameraControllerComponent::StaticClass());
			//	if (CameraController) Cast<UPlayerCameraControllerComponent>(CameraController)->SetCameraBounds(*GetGameraBoundsBox());

			//	// Run method on Blueprint class implementation
			//	OnRunRoomEvents();

			//	Character->SetCurrentRoomBounds(*GetRoomBoundsBox());
			//}
	//	}
	//	else if (ABasePaperCharacter* Character = Cast<ABasePaperCharacter>(OtherActor) ) { // TODO: Could change to Enemy/ NPC?
	//		// TODO: Store NPC character references in array?
	//		Character->SetCurrentRoomBounds(*GetRoomBoundsBox());
	//	}
	}
}

void ABaseRoom::OnComponentExitRoom(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	//if (OtherActor->IsA(ABasePaperCharacter::StaticClass())) {
	//	if (APlayerPaperCharacter* Character = Cast<APlayerPaperCharacter>(OtherActor)) {
			//PlayerCharactersInRoom.Remove(Character);

			//if (PlayerCharactersInRoom.Num() == 0) OnEndRoomEvents();
		//}
		//else if (ABasePaperCharacter* Character = Cast<ABasePaperCharacter>(OtherActor)) { // TODO: Could change to Enemy/ NPC?
		//	//Remove NPC ref from array?																			   // TODO: Store NPC character references in array?																		   // TODO: Store NPC character references in array?
		//	Character->SetCurrentRoomBounds(*GetRoomBoundsBox());
		//}
	//}
}

bool ABaseRoom::AddNeighbour( ABaseDoor* Door,  ABaseRoom * Neighbour) {
	UE_LOG(LogTemp, Warning, TEXT("ABaseRoom::AddNeighbour"));
	if (Door != nullptr) {
		if (Neighbour != nullptr) {
			
			if (Door->GetOwner() == this) {
				UE_LOG(LogTemp, Warning, TEXT("Adding Neighbour..."));
				// door->GetNeighbouringDoorAndOwner ...
				DoorsToNeighbourRooms.Add(Door, Neighbour);

				return true;
			}
			else return false;
		}
		else return false;
	}
	else return false;
}


bool ABaseRoom::PassCharacterToNeighbour(ABaseDoor * DoorToUse, ABasePaperCharacter* CharacterToSend, FVector LocationToMoveTo) {
	if (DoorsToNeighbourRooms.Num() > 0) {
		ABaseRoom* Neighbour = DoorsToNeighbourRooms[DoorToUse];

		if (Neighbour) {
			if (Neighbour->AddCharacterToRoomPop(CharacterToSend)) {

				TPair<class ABasePaperCharacter*, bool> NewPair;
				NewPair.Key = CharacterToSend;
				NewPair.Value = CharacterToSend->IsA(APlayerPaperCharacter::StaticClass());

				CharactersInRoom.Remove(NewPair);

				OnCharacterExitDelegate.Broadcast(Cast<ABasePaperCharacter>(CharacterToSend), PlayersInRoomCount);
				CharacterToSend->ForceMoveToLocation(LocationToMoveTo);

				// Decrease the player Count
				if (NewPair.Value) --PlayersInRoomCount;
			}
		}
	}

	return false;
}

bool ABaseRoom::AddCharacterToRoomPop(ABasePaperCharacter * CharacterToAdd) { 
	UE_LOG(LogTemp, Warning, TEXT("ABaseRoom::AddCharacterToRoomPop"));

	// Run method on Blueprint class implementation
	if (PlayersInRoomCount == 0) OnRunRoomEvents();
	
		TPair<class ABasePaperCharacter*, bool> NewPair;
		NewPair.Key = CharacterToAdd;
		NewPair.Value = CharacterToAdd->IsA(APlayerPaperCharacter::StaticClass());

		CharactersInRoom.Add(NewPair);

		auto* CameraController = CharacterToAdd->GetComponentByClass(UPlayerCameraControllerComponent::StaticClass());
		if (CameraController) Cast<UPlayerCameraControllerComponent>(CameraController)->SetCameraBounds(*GetGameraBoundsBox());

		CharacterToAdd->SetCurrentRoom(this);

		// TODO: Check whether this is still necessary for AI
		// If the character is not a player, then pass room bounds
		if (!NewPair.Value) {
			if (ABasePaperCharacter* Character = Cast<ABasePaperCharacter>(CharacterToAdd)) {
				Character->SetCurrentRoomBounds(*GetRoomBoundsBox());
			}
		}
		// Increase the player count
		else {
			++PlayersInRoomCount;
			OnCharacterEnterDelegate.Broadcast(Cast<ABasePaperCharacter>(CharacterToAdd), PlayersInRoomCount);
		}
	return true;
}
