// Fill out your copyright notice in the Description page of Project Settings.

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
#include "GameFramework/SpringArmComponent.h"

#include "Components/PlayerCameraControllerComponent.h"

#include "SpriteObjects/PlayerPaperCharacter.h"

const float CAMERA_ANGLE = -60.0f; // TODO: Make global

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

	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ABaseRoom::BeginPlay() {
	Super::BeginPlay();

	// Get the dimensions of the room mesh and use it to 
	//define the trigger box and camera bounds for the room
	FVector RoomDimensions = RoomMesh->GetStaticMesh()->GetBounds().GetBox().GetSize();
	RoomDimensions = RoomDimensions / 2;
	if (RoomBounds != nullptr) RoomBounds->SetBoxExtent(RoomDimensions * 0.7f);
	if (CameraBounds !=  nullptr) CameraBounds->SetBoxExtent(RoomDimensions * 0.7f);
}


// Called every frame
void ABaseRoom::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

/*The intended use of this method is to detect the player character entering the room, at which point the room will initiate its event sequence. */
void ABaseRoom::OnComponentEnterRoom(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) {
			
	//TODO: Restructure the enter and leave methods
	// - Discern if character > then type of character  > then what to do with each type

	if (OtherActor->IsA(ABasePaperCharacter::StaticClass())) {

		if (APlayerPaperCharacter* Character = Cast<APlayerPaperCharacter>(OtherActor)) {

			if (!PlayerCharactersInRoom.Contains(Character)) {

				PlayerCharactersInRoom.Add(Character);
				OnCharacterEnterDelegate.Broadcast(Cast<ABasePaperCharacter>(OtherActor), PlayerCharactersInRoom.Num());

				auto* CameraController = OtherActor->GetComponentByClass(UPlayerCameraControllerComponent::StaticClass());
				if (CameraController) Cast<UPlayerCameraControllerComponent>(CameraController)->SetCameraBounds(*GetGameraBoundsBox());

				// Run method on Blueprint class implementation
				OnRunRoomEvents();

				Character->SetCurrentRoomBounds(*GetRoomBoundsBox());
			}
		}
		else if (ABasePaperCharacter* Character = Cast<ABasePaperCharacter>(OtherActor) ) { // TODO: Could change to Enemy/ NPC?
			// TODO: Store NPC character references in array?
			Character->SetCurrentRoomBounds(*GetRoomBoundsBox());
		}
	}
}

void ABaseRoom::OnComponentExitRoom(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor->IsA(ABasePaperCharacter::StaticClass())) {
		if (APlayerPaperCharacter* Character = Cast<APlayerPaperCharacter>(OtherActor)) {
			PlayerCharactersInRoom.Remove(Character);
			OnCharacterExitDelegate.Broadcast(Cast<ABasePaperCharacter>(OtherActor), PlayerCharactersInRoom.Num());

			if (PlayerCharactersInRoom.Num() == 0) OnEndRoomEvents();
		}
		//else if (ABasePaperCharacter* Character = Cast<ABasePaperCharacter>(OtherActor)) { // TODO: Could change to Enemy/ NPC?
		//	//Remove NPC ref from array?																			   // TODO: Store NPC character references in array?																		   // TODO: Store NPC character references in array?
		//	Character->SetCurrentRoomBounds(*GetRoomBoundsBox());
		//}
	}
}


