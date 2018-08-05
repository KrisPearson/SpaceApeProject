// // Copyright 2018 Kristiam Pearson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Curves/CurveVector.h"
#include "Components/TimelineComponent.h"
#include "BaseDoor.generated.h"

UCLASS()
class SPACEAPEPROJECT_API ABaseDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



	//UPROPERTY()
		//class UTimelineComponent* DoorAnimationTimeline;




	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door")
		class UBoxComponent* TriggerBox;

	UPROPERTY(BlueprintReadOnly, Category = "Door")
		class UStaticMeshComponent* BlockingMesh;

	//UStaticMeshComponent* DoorFrame;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door")
		UStaticMeshComponent* DoorMeshLeft;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door")
		UStaticMeshComponent* DoorMeshRight;

	// The other DoorComponent to which this door is linked
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door") // TEMP ACCESS
		ABaseDoor* TwinDoor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Door")
		UCurveVector* DoorAnimationCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Door") // TODO: Swap the world locations of both door target locations
		USceneComponent* MoveToLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door")
		class UArrowComponent* DoorDirectionArrow;

	USceneComponent* RootComponent;


	// Variable used to store initial mesh location
	FVector DoorOffset;


	UFUNCTION()
		void OnComponentEnterTrigger(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult &SweepResult);

	UFUNCTION()
		void OnComponentExitTrigger(UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable)
		inline  bool GetIsLocked() { return bIsLocked; };


	FTimeline DoorAnimationTimeline;



private:

	UFUNCTION()
	void AnimateDoor();


	UFUNCTION()
	void SetDoorState();

	void ToggleDoor();

	void FindTwinDoor();



	TArray<class APlayerPaperCharacter*> PlayerCharactersInRoom;


	float TimelineValue;

	FVector DoorMeshPositionValue;

	bool bIsOpenState = false;

	bool bIsLocked = false;


};
