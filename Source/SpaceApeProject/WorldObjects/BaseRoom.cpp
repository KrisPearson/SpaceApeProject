// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseRoom.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/SpringArmComponent.h"

#include "SpriteObjects/PlayerPaperCharacter.h"

const float CAMERA_ANGLE = -60.0f; // TEMP

// Sets default values
ABaseRoom::ABaseRoom() {

	RoomMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Room Mesh"));

	CameraBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Camera Bounds Box"));
	CameraBounds->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CameraBounds->SetupAttachment(RoomMesh);

	RoomTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Room Trigger Box"));
	RoomTriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	RoomTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseRoom::OnComponentEnterRoom);
	RoomTriggerBox->SetupAttachment(RoomMesh);



	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	//CameraBoom->TargetArmLength = 500.0f;
	//CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 120.0f);
	////CameraBoom->bAbsoluteRotation = true;
	//CameraBoom->bDoCollisionTest = false;
	//CameraBoom->RelativeRotation = FRotator(CAMERA_ANGLE, 0.0f, 0.0f);





	PrimaryActorTick.bCanEverTick = true;

	//https://answers.unrealengine.com/questions/239179/old-zelda-style-camera.html

}

// Called when the game starts or when spawned
void ABaseRoom::BeginPlay() {
	Super::BeginPlay();

	FVector RoomDimensions = RoomMesh->GetStaticMesh()->GetBounds().GetBox().GetSize();
	RoomDimensions = RoomDimensions / 2;
	if (RoomTriggerBox != nullptr) RoomTriggerBox->SetBoxExtent(RoomDimensions);
	if (CameraBounds !=  nullptr) CameraBounds->SetBoxExtent(RoomDimensions * 0.65f);
	else UE_LOG(LogTemp, Warning, TEXT("ABaseRoom::CameraBounds = nullptr"));
}


// Called every frame
void ABaseRoom::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);


}

void ABaseRoom::OnComponentEnterRoom(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) {
	if (OtherActor->IsA(APlayerPaperCharacter::StaticClass())) {
		// Inform character interace of room change
		// Pass new camera arm component reference
		// Trigger Room Sequence



		Cast<APlayerPaperCharacter>(OtherActor)->SetCameraBounds(CameraBounds);

		OnRunRoomEvents();
	}
}


