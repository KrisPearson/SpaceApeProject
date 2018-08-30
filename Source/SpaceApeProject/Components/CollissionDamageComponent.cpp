// // Copyright 2018 Kristiam Pearson. All Rights Reserved.

#include "CollissionDamageComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Classes/GenericTeamAgentInterface.h"
#include "Interfaces/DamageableInterface.h"

// Sets default values for this component's properties
UCollissionDamageComponent::UCollissionDamageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


void UCollissionDamageComponent::InitialiseComponent(UShapeComponent & Collider, UBoxComponent & OverlapDetector) {
	this->Collider = &Collider;
	this->OverlapDetector = &OverlapDetector;
}

// Called when the game starts
void UCollissionDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Collider) Collider->OnComponentHit.AddDynamic(this, &UCollissionDamageComponent::OnHit);
	
	OverlapDetector->OnComponentBeginOverlap.AddDynamic(this, &UCollissionDamageComponent::OnComponentEnterTrigger);
	OverlapDetector->OnComponentEndOverlap.AddDynamic(this, &UCollissionDamageComponent::OnComponentExitTrigger);

}


// Called every frame
void UCollissionDamageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCollissionDamageComponent::OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit) {
	IDamageableInterface* ObjectInterface = Cast<IDamageableInterface>(OtherActor);
	IGenericTeamAgentInterface* TeamInterface = Cast<IGenericTeamAgentInterface>(GetOwner());
	if (ObjectInterface && TeamInterface) {
		if (!ActorsCollidedWith.Contains(OtherActor)) {


			UE_LOG(LogTemp, Warning, TEXT(" ABasePaperCharacter::OnHit  "));


			IDamageableInterface::Execute_ReceiveDamage(OtherActor, CollissionDamage, GetOwner(), TeamInterface->GetGenericTeamId() ); // TODO GetOwnerDamage

			ActorsCollidedWith.Add(OtherActor);

		}

	}
}

void UCollissionDamageComponent::OnComponentEnterTrigger(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	// do nothing
}

void UCollissionDamageComponent::OnComponentExitTrigger(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex) {
	// remove the actor from array
	if (ActorsCollidedWith.Contains(OtherActor)) ActorsCollidedWith.Remove(OtherActor);
}

