// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PlayerCameraControllerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEAPEPROJECT_API UPlayerCameraControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerCameraControllerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** Camera boom positioning the camera above the character */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* Camera;



public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	void SetCameraBounds(const class UBoxComponent& CameraBoundsBox);

	void SetBoomReference(class USpringArmComponent& NewSpringArm);

	float CameraInterpSpeed = 1.5f;

private:
	void UpdateCameraBounds(float DeltaTime);

	// The magnitude applied to the offset of the player camera in the facing direction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	int OffsetMagnitude = 100;

	FVector CameraBoundsMin;
	FVector CameraBoundsMax;
};
