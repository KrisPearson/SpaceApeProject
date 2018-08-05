// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseRoom.generated.h"




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


	FOnCharacterEnter OnCharacterEnterDelegate;

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

	TArray<class APlayerPaperCharacter*> PlayerCharactersInRoom;

public:	

	UBoxComponent * const GetGameraBoundsBox() const { return CameraBounds; };

	UBoxComponent * const GetRoomBoundsBox() const { return RoomBounds; };
	
};
