// Fill out your copyright notice in the Description page of Project Settings.

#include "SpriteShadowComponent.h"
#include "Engine/DecalActor.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h" 



// Sets default values for this component's properties
USpriteShadowComponent::USpriteShadowComponent()
{

	static ConstructorHelpers::FObjectFinder<UMaterial> ShadowMaterialFinder(TEXT("/Game/Materials/SpriteShadow_Mat.SpriteShadow_Mat") );
	ShadowMaterial = ShadowMaterialFinder.Object;

	PrimaryComponentTick.bCanEverTick = true;
	//SetComponentTickInterval(0.001f);

	ShadowScale = 0.2f;
}

// Called when the game starts
void USpriteShadowComponent::BeginPlay() {
	Super::BeginPlay();

	ShadowDecal = GetWorld()->SpawnActor<ADecalActor>();
	if (ShadowMaterial != nullptr) ShadowDecal->SetDecalMaterial(ShadowMaterial);
	else UE_LOG(LogTemp, Warning, TEXT("Failed to load ShadowMaterial for SpriteShadowComponent"));
	ShadowDecal->SetActorScale3D(FVector(0.3, ShadowScale, ShadowScale));
}


// Called every frame
void USpriteShadowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateDecal(DeltaTime);

	
}

// Updates the decals position to follow a location beneath the owningAcotr
void USpriteShadowComponent::UpdateDecal(float DeltaTime) {
	if (ShadowDecal != nullptr) {
		FVector Location = GetOwner()->GetActorLocation();

		UWorld* World = GetWorld();
		if (World != nullptr) {

			//Re-initialize hit info
			FHitResult HitOut;

			// Start on the owner's location, and end 10 units below the owner
			const FVector Start = Location;
			const FVector End = Start + FVector(0, 0, -200);

			//The trace data is stored here
			FHitResult HitData(ForceInit);


//			ETraceQueryType MyTraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2);

			ECollisionChannel CollisionChannel = ECC_GameTraceChannel6;//ECC_WorldStatic;
			
			FCollisionQueryParams CollisionParams = FCollisionQueryParams( FName(TEXT("ShadowTrace")), true, GetOwner() );
			//CollisionParams.AddIgnoredActor(GetOwner());
			
			World->LineTraceSingleByChannel(
				HitOut,
				Start,
				End,
				CollisionChannel,
				CollisionParams
			);
			// Check whether anything was hit
			if (HitOut.GetActor() != NULL) {
				//HitOut.Distance; ///TODO: Use distance to scale or "fade in/ out" the shadow. Use a min and max distance to scale from 0 to ShadowScale
				Location.Z = HitOut.ImpactPoint.Z; // Set the Z position of the Decal to that of the point hit on the other actor
				

				//if (HitOut.Actor->GetFName().ToString() != "Floor") UE_LOG(LogTemp, Warning, TEXT("HitOut = %s"), *HitOut.Actor->GetFName().ToString());
			}
			else {
				Location.Z = Location.Z - 70;
				//UE_LOG(LogTemp, Warning, TEXT("HitOut == null"));
			}
			//ShadowDecal->SetActorLocation(Location);


			TargetLocation = Location;
		}
	}
	//ShadowDecal->SetActorLocation(FMath::VInterpTo(ShadowDecal->GetActorLocation(), TargetLocation, DeltaTime, 1000));
	ShadowDecal->SetActorLocation(FMath::VInterpConstantTo(ShadowDecal->GetActorLocation(), TargetLocation, DeltaTime, 1000));
}

void USpriteShadowComponent::SetShadowScale(float NewScale) {
	ShadowScale = NewScale;
	ShadowDecal->SetActorScale3D(FVector(0.3, ShadowScale, ShadowScale)); 
	//TODO: Lerp to the new scale?
}

