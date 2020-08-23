// // Copyright 2018 Kristian Pearson. All Rights Reserved.

#include "BaseDoor.h"
#include "Components/BoxComponent.h"
#include "Components//StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/PlayerCameraControllerComponent.h"
#include "WorldObjects/BaseRoom.h"

#include "SpriteObjects/PlayerPaperCharacter.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"


//TODO: Make different door types derrive from this class (eg single, double, slide down etc)

// Sets default values
ABaseDoor::ABaseDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComponent);


	MoveToLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MoveToLocation"));
	MoveToLocation->AttachTo(RootComponent);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorTriggerBox"));
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	TriggerBox->AttachTo(RootComponent);

	DoorMeshLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMeshleft"));
	DoorMeshLeft->SetupAttachment(RootComponent);

	DoorMeshRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMeshRight"));
	DoorMeshRight->SetupAttachment(RootComponent);


	DoorDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FaceDirectionArrow"));
	DoorDirectionArrow->SetupAttachment(RootComponent);

	BlockingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockingVolume"));;
	BlockingMesh->SetStaticMesh(DoorMeshLeft->GetStaticMesh());
	BlockingMesh->AttachTo(RootComponent);
}

// Called when the game starts or when spawned
void ABaseDoor::BeginPlay() {
	Super::BeginPlay();

	if (TriggerBox) {
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseDoor::OnComponentEnterTrigger);
		TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ABaseDoor::OnComponentExitTrigger);
	}

	if (DoorAnimationCurve) {
			FOnTimelineVector TimelineCallback;
			FOnTimelineEventStatic TimelineFinishedCallback;

			TimelineCallback.BindUFunction(this, FName("AnimateDoor"));
			TimelineFinishedCallback.BindUFunction(this, FName("SetDoorState"));

			DoorAnimationTimeline.AddInterpVector(DoorAnimationCurve, TimelineCallback);
			DoorAnimationTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);

	}
	if (DoorMeshLeft != nullptr && BlockingMesh != nullptr) {
		BlockingMesh->SetStaticMesh(DoorMeshLeft->GetStaticMesh());
		BlockingMesh->SetVisibility(false);
		BlockingMesh->SetWorldLocation(DoorMeshLeft->GetComponentLocation());
		BlockingMesh->SetWorldRotation(DoorMeshLeft->GetComponentRotation());
		BlockingMesh->SetCollisionProfileName(TEXT("BlockAll"));


		DoorOffset = DoorMeshLeft->GetRelativeTransform().GetLocation();
	}
}
 
ABaseRoom * ABaseDoor::GetOwningRoom() {
	if (GetOwner()) {
		UE_LOG(LogTemp, Warning, TEXT("ABaseDoor::GetOwningRoom. Neighbour = %s"), *(GetOwner()->GetFName().ToString()));

		return Cast<ABaseRoom>(GetOwner());
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("ABaseDoor::GetOwningRoom - Owner == nullptr"));
		return nullptr;
	}
}

void ABaseDoor::AddCharacterToIncomingCharacters(ABasePaperCharacter * IncomingCharacter) {
	UE_LOG(LogTemp, Warning, TEXT("ABaseDoor::AddCharacterToIncomingCharacters"));
	IncomingCharacters.Add(IncomingCharacter);
	//if (!bIsOpenState) 
		ToggleDoor();
}

void ABaseDoor::AnimateDoor() {
	TimelineValue = DoorAnimationTimeline.GetPlaybackPosition();
	DoorMeshPositionValue = DoorAnimationCurve->GetVectorValue(TimelineValue);

	DoorMeshLeft->SetRelativeLocation((DoorMeshPositionValue - DoorOffset));
	DoorMeshRight->SetRelativeLocation((DoorMeshPositionValue - DoorOffset) * -1);
}

void ABaseDoor::OnComponentEnterTrigger(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {

	if (OtherActor->IsA(ABasePaperCharacter::StaticClass())) {

		ABasePaperCharacter* Character = Cast<ABasePaperCharacter>(OtherActor);

		// Remove the character from the incoming characters array; completing character room transfer
		if (IncomingCharacters.Contains(OtherActor)) IncomingCharacters.Remove(Character);

		if (TwinDoor == nullptr) {
			if (FindTwinDoor()) {
				GetOwningRoom()->AddNeighbour(this, TwinDoor->GetOwningRoom());
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("No Twin found - aborting move"));
				return; // No Twin Found
			}
		}

		OutgoingCharacters.Add(Character);

		if (!bIsOpenState) ToggleDoor();




		//OutgoingCharacters.Add(Character);
		//OnCharacterEnterDelegate.Broadcast(Cast<ABasePaperCharacter>(OtherActor), OutgoingCharacters.Num());

		//auto* CameraController = OtherActor->GetComponentByClass(UPlayerCameraControllerComponent::StaticClass());
		//if (CameraController) Cast<UPlayerCameraControllerComponent>(CameraController)->SetCameraBounds(*GetGameraBoundsBox());

		// Run method on Blueprint class implementation
		//OnRunRoomEvents();

		//Character->SetCurrentRoomBounds(*GetRoomBoundsBox());
	}
}

void ABaseDoor::OnComponentExitTrigger(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex) {
	if (OtherActor->IsA(ABasePaperCharacter::StaticClass())) {


		if (APlayerPaperCharacter* Character = Cast<APlayerPaperCharacter>(OtherActor)) {

			OutgoingCharacters.Remove(Character);

			ToggleDoor();

			//OutgoingCharacters.Remove(Character);
			//OnCharacterExitDelegate.Broadcast(Cast<ABasePaperCharacter>(OtherActor), OutgoingCharacters.Num());

			//if (OutgoingCharacters.Num() == 0) OnEndRoomEvents();
		}
		//else if (ABasePaperCharacter* Character = Cast<ABasePaperCharacter>(OtherActor)) { // TODO: Could change to Enemy/ NPC?
		//	//Remove NPC ref from array?																			   // TODO: Store NPC character references in array?																		   // TODO: Store NPC character references in array?
		//	Character->SetCurrentRoomBounds(*GetRoomBoundsBox());
		//}
	}
}

// Called every frame
void ABaseDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DoorAnimationTimeline.TickTimeline(DeltaTime);
	//DoorAnimationTimeline.TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);

}

void ABaseDoor::SetDoorState() {

	bIsOpenState = TimelineValue > 0 ? true : false;

	if (bIsOpenState) {

		TArray<ABasePaperCharacter*> CharactersToMove;

		BlockingMesh->SetCollisionProfileName(TEXT("OverlapAll"));

		for (ABasePaperCharacter* Char : OutgoingCharacters) {

			CharactersToMove.Add(Char);
		}
		if (TwinDoor) {
			FVector TempLoc = TwinDoor->MoveToLocation->GetComponentLocation();

			for (ABasePaperCharacter* Char : CharactersToMove) {
				//Char->ForceMoveToLocation(TempLoc);
				TwinDoor->AddCharacterToIncomingCharacters(Char);
				GetOwningRoom()->PassCharacterToNeighbour(this, Char, TempLoc);

				//TransitiioningCharacters.Add(Char);
			}
		}
	}
	else {
		BlockingMesh->SetCollisionProfileName(TEXT("BlockAll"));
	}



	//if (TimelineValue == 0) {
	//	bIsOpenState = false;
	//}
	//else if (TimelineValue > 0) {
	//	bIsOpenState = true;
	//}
}

void ABaseDoor::ToggleDoor() {

	//if (TwinDoor) {
		if (IncomingCharacters.Num() > 0) {
			UE_LOG(LogTemp, Warning, TEXT("ABaseDoor::ToggleDoor IncomingCharacters  > 0"));
		}

		if (OutgoingCharacters.Num() > 0 || IncomingCharacters.Num() > 0 ) { // open the door
			
			if (!bIsLocked) {
				if (!bIsOpenState) {
					DoorAnimationTimeline.PlayFromStart();

					//if (TwinDoor) TwinDoor->DoorAnimationTimeline.PlayFromStart();
				}
			}
		}
		else {
			DoorAnimationTimeline.Reverse();

			//if (TwinDoor) TwinDoor->DoorAnimationTimeline.Reverse();
		}
	//}
}

bool ABaseDoor::FindTwinDoor() {

	UWorld* World = GetWorld();
	if (World != nullptr) {

		FHitResult HitOut;

		// Start on the owner's location, and end 10 units below the owner
		const FVector Start = DoorDirectionArrow->GetComponentLocation();
		const FVector End = Start + ((DoorDirectionArrow->GetForwardVector() * -1) * 1000/*Distance*/);

		//The trace data is stored here
		FHitResult HitData(ForceInit);
		ECollisionChannel CollisionChannel = ECC_WorldStatic;

		FCollisionQueryParams CollisionParams;// = FCollisionQueryParams(FName(TEXT("ShadowTrace")), true, GetOwner());
		CollisionParams.AddIgnoredActor(this);

		World->LineTraceSingleByChannel(
			HitOut,
			Start,
			End,
			CollisionChannel,
			CollisionParams
		);

		// Check whether anything was hit
		if (HitOut.GetActor() != NULL) {
			if (ABaseDoor* OtherDoor = Cast<ABaseDoor>(HitOut.GetComponent()->GetOwner())) {
				//UE_LOG(LogTemp, Warning, TEXT("Door Hitout component = %s"), *(HitOut.GetComponent()->GetFName().ToString()));
				//UE_LOG(LogTemp, Warning, TEXT("Component owner name = %s"), *(HitOut.GetComponent()->GetOwner()->GetFName().ToString()));
				//UE_LOG(LogTemp, Warning, TEXT("This Door Name = %s"), *(this->GetFName().ToString()));
				DrawDebugLine(GetWorld(), Start, End, FColor::Emerald, true, -1, 0, 20);
				TwinDoor = OtherDoor;
				return true;
			}
			else return false;
		}
		else return false;
	}
	else return false;
}