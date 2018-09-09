// // Copyright 2018 Kristiam Pearson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CollissionDamageComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEAPEPROJECT_API UCollissionDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCollissionDamageComponent();

	void InitialiseComponent(class UShapeComponent & Collider, class UBoxComponent & OverlapDetector);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;




	int CollissionDamage = 5; // TEMP ??

	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FTimerHandle CollissionInvulnerabilityTimer;

	class UShapeComponent* Collider;

	class UBoxComponent* OverlapDetector;

	UFUNCTION()
		void OnComponentExitTrigger(UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

private:

	// Stores references to the actors that have already been collided with
	TArray<AActor*> ActorsCollidedWith;

		
	
};
