// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Map.h"
#include "BaseRoom.generated.h"


class ABaseDoor;
class ABasePaperCharacter;


//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerEnter, class ABasePaperCharacter*, CharacterPointer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterEnter, class ABasePaperCharacter*, EnteringCharacter, int32, NumPlayerCharactersInRoom);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterExit, class ABasePaperCharacter*, EnteringCharacter, int32, NumPlayerCharactersInRoom);

UCLASS()
class SPACEAPEPROJECT_API ABaseRoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseRoom();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* Used to indicate to components of the Room the presence of characters*/
	FOnCharacterEnter OnCharacterEnterDelegate;

	/* Used to indicate to components of the Room the presence of characters*/
	FOnCharacterExit OnCharacterExitDelegate;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// The Bounds for this room for Characters
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Room)
	class UBoxComponent* RoomBounds;

	// The Bounds for this Room of the Camera
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Room)
	class UBoxComponent* CameraBounds;

	// The component to which the player's camera will be attached upon entering the RoomTriggerBox
	//class USpringArmComponent* CameraBoom;

	// The Primary mesh for the room
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Room)
		class UStaticMeshComponent* RoomMesh;

	// The Material to apply to the room
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Room)
		//class Material* TileSet;



	UFUNCTION()
	void OnComponentEnterRoom(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult &SweepResult);

	UFUNCTION()
		void OnComponentExitRoom(UPrimitiveComponent* OverlappedComp, 
			AActor* OtherActor, 
			UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex);

	void BeginRoomEvents();

	// Method to be used by Blueprint implementations of this class
	UFUNCTION(BlueprintImplementableEvent, Category = "Room Event")
	void OnRunRoomEvents();

	// Method to be used by Blueprint implementations of this class. Called when the 
	UFUNCTION(BlueprintImplementableEvent, Category = "Room Event")
		void OnEndRoomEvents();

	//TArray<class APlayerPaperCharacter*> PlayerCharactersInRoom;

	/*Stores the character and whether the character is player controlled*/
	TArray<TPair<ABasePaperCharacter*, bool>> CharactersInRoom;

	int PlayersInRoomCount = 0;

	/*Store a collection of doors and the neighbouring rooms to which they link.
	Using this, the room has the capacity to transfer the player to the other room. */
	TMap<ABaseDoor*, ABaseRoom*> DoorsToNeighbourRooms;



	int CountPlayersInRoom() {
		int Count = 0;

		for (int i = 0; i < CharactersInRoom.Num(); i++) {
			if ( CharactersInRoom[i].Value ) ++Count;
		}
		return Count;
	}


public:	

	/* Add this rooms door and its neighbour to the Neighbours Map */
	bool AddNeighbour(ABaseDoor* Door, ABaseRoom* Neighbour);

	bool PassCharacterToNeighbour(ABaseDoor* DoorToUse, ABasePaperCharacter* CharacterToSend);

	bool AddCharacterToRoomPop(ABasePaperCharacter* CharacterToAdd);


	UBoxComponent * const GetGameraBoundsBox() const { return CameraBounds; };

	UBoxComponent * const GetRoomBoundsBox() const { return RoomBounds; };
	
};
