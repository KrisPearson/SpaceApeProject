// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpriteShadowComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEAPEPROJECT_API USpriteShadowComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpriteShadowComponent();



	// Sets the dimensions of the shadow for this component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shadow, meta = (AllowPrivateAccess = "true"))
	float ShadowScale;



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//virtual void BeginDestroy() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void UpdateDecal(float DeltaTime);

	FVector TargetLocation;

	// The decal projected beneath the sprite
	class ADecalActor* ShadowDecal;

	class UDecalComponent* ShadowDecalComponent;

	// The material to be applied to the ShadowDecal
	class UMaterial* ShadowMaterial;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetShadowScale(float NewScale);
};
 