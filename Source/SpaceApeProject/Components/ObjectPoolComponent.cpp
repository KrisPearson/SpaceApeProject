// Fill out your copyright notice in the Description page of Project Settings.

/*
ObjectPoolComponent.cpp
Purpose: The primary purpose of this object pool is to handle projectiles, but it is generic enough to permit 
other AActor derived types to be stored.

@author Kristian Pearson
@version 1.0 17/18/2018
*/

#include "ObjectPoolComponent.h"

// Sets default values for this component's properties
UObjectPoolComponent::UObjectPoolComponent() {

}

void UObjectPoolComponent::FillPool(TSubclassOf<AActor> _ActorToSpawn, int _Count) {
	PooledObjects.Reserve(_Count); // Reserves memory for the array

	if (UWorld* World = GetWorld()) {

		for (int i = 0; i < _Count; i++) {

			SpawnActorForPool(_ActorToSpawn);

			//UE_LOG(LogTemp, Warning, TEXT(" Adding Object to Pool"));

		}
	}
}

AActor* UObjectPoolComponent::GetReusableReference() {
	if (PooledObjects.Num() <= 1) {
		/* Commented out functionality would add another object to the pool. The issue is that new objects wouldn't be bound to functions etc.
		UE_LOG(LogTemp, Warning, TEXT(" Adding new obj to pool"));
		SpawnActorForPool(PooledObjectsType);

		//if (UWorld* World = GetWorld()) { AActor* newObject = World->SpawnActor< AActor >(PooledObjectsType); }
		//PooledObjects.Add(newObject);
		//newObject->SetActorLocation(FVector(0, 200, 300));
		//return newObject;
		*/

		UE_LOG(LogTemp, Warning, TEXT(" GetReusableReference returned nullptr"));
		return nullptr;
	}
	else {
		AActor* returnActor = PooledObjects.Top();
		//UE_LOG(LogTemp, Warning, TEXT(" GetReusableReference  = %s"), *returnActor->GetName());
		PooledObjects.Pop();
		CurrentNumberOfPooledObjectsInUse++;

		return returnActor;
	}
}

void UObjectPoolComponent::ReturnReusableReference(AActor * _Ref) {
	//UE_LOG(LogTemp, Warning, TEXT("Return object to pool %s"), *_Ref->GetName());
	CurrentNumberOfPooledObjectsInUse--;
	_Ref->Reset();
	PooledObjects.Push(_Ref);
}


/*
This method is used to increase the number of objects in the pool post initialisation
*/
void UObjectPoolComponent::SpawnActorForPool_Implementation(TSubclassOf<AActor> _ActorToSpawn) {
	AActor* NewObject = GetWorld()->SpawnActor< AActor >(_ActorToSpawn, FVector(0,0,-500), FRotator(0,0,0));
	PooledObjects.Add(NewObject);
}

bool UObjectPoolComponent::SpawnActorForPool_Validate(TSubclassOf<AActor> _ActorToSpawn) {
	return true;
}

