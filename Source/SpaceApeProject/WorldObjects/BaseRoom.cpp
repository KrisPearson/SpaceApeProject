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

	RoomTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Room Trigger Box"));
	RoomTriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	RoomTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseRoom::OnComponentEnterRoom);
	RoomTriggerBox->OnComponentEndOverlap.AddDynamic(this, &ABaseRoom::OnComponentExitRoom);
	RoomTriggerBox->SetupAttachment(RoomMesh);

	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABaseRoom::BeginPlay() {
	Super::BeginPlay();

	// Get the dimensions of the room mesh and use it to 
	//define the trigger box and camera bounds for the room
	FVector RoomDimensions = RoomMesh->GetStaticMesh()->GetBounds().GetBox().GetSize();
	RoomDimensions = RoomDimensions / 2;
	if (RoomTriggerBox != nullptr) RoomTriggerBox->SetBoxExtent(RoomDimensions);
	if (CameraBounds !=  nullptr) CameraBounds->SetBoxExtent(RoomDimensions * 0.5f);
}


// Called every frame
void ABaseRoom::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

/*The intended use of this method is to detect the player character entering the room, at which point the room will initiate its event sequence. */
void ABaseRoom::OnComponentEnterRoom(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) {
	
	if (OtherActor->IsA(APlayerPaperCharacter::StaticClass())) {

		auto* CameraController = OtherActor->GetComponentByClass(UPlayerCameraControllerComponent::StaticClass());
		if (CameraController) Cast<UPlayerCameraControllerComponent>(CameraController)->SetCameraBounds(*CameraBounds);
		//Cast<APlayerPaperCharacter>(OtherActor)->SetCameraBounds(*CameraBounds); // Pass to componen<=== TODO!!!!

		OnRunRoomEvents();
	}
}

void ABaseRoom::OnComponentExitRoom(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor->IsA(APlayerPaperCharacter::StaticClass())) {
		// TODO: turn off lights

		//Cast<APlayerPaperCharacter>(OtherActor)->SetCameraBounds(nullptr);
	}
}


