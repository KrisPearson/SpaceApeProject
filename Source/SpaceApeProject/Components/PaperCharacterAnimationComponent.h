// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PaperCharacterAnimationComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEAPEPROJECT_API UPaperCharacterAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPaperCharacterAnimationComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	void PlayFlipbookAnimation(/*UPaperFlipbook FlipbookToPlay, bool OneShot, float Duration*/ );

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

#pragma region Sprite Animations

	// The animations to play while walking around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Walk")
		class UPaperFlipbook* WalkRightFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Walk")
		class UPaperFlipbook* WalkLeftFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Walk")
		class UPaperFlipbook* WalkUpFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Walk")
		class UPaperFlipbook* WalkDownFlipbook;

	// The animations to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Idle")
		class UPaperFlipbook* IdleRightFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Idle")
		class UPaperFlipbook* IdleLeftFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Idle")
		class UPaperFlipbook* IdleUpFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations|Idle")
		class UPaperFlipbook* IdleDownFlipbook;

#pragma endregion

private:
	class APlayerPaperCharacter* OwningCharacter;
	
};
