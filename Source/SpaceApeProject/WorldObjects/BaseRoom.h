// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseRoom.generated.h"

UCLASS()
class SPACEAPEPROJECT_API ABaseRoom : public AActor
{
	GENERATED_BODY()
	

	//TArray<SpawnComponents>

public:	
	// Sets default values for this actor's properties
	ABaseRoom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Room)
	class UBoxComponent* RoomTriggerBox;

	// The Bounds for this Room of the Camera
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Room)
	class UBoxComponent* CameraBounds;

	// The component to which the player's camera will be attached upon entering the RoomTriggerBox
	//class USpringArmComponent* CameraBoom;

	// The Primary mesh for the room
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Room)
		class UStaticMeshComponent* RoomMesh;

	// The Primary mesh for the room
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

	UFUNCTION(BlueprintImplementableEvent, Category = "Room Event")
	void OnRunRoomEvents();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
