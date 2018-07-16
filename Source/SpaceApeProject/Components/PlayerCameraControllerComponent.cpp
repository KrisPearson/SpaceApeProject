// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCameraControllerComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interfaces/SpriteCharacterInterface.h"

const float CAMERA_ANGLE = -60.0f;


// Sets default values for this component's properties
UPlayerCameraControllerComponent::UPlayerCameraControllerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->TargetArmLength = 500.0f;
	//CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 120.0f);
	////CameraBoom->bAbsoluteRotation = true;
	//CameraBoom->bDoCollisionTest = false;
	//CameraBoom->RelativeRotation = FRotator(CAMERA_ANGLE, 0.0f, 0.0f);

	// Create an orthographic camera (no perspective) and attach it to the boom
	//Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ObliqueViewCamera"));
	//Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
	//Camera->OrthoWidth = 2048.0f;
	//Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// ...
}


// Called when the game starts
void UPlayerCameraControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	//CameraBoom = NewObject<USpringArmComponent>(GetOuter(), USpringArmComponent::StaticClass(), TEXT("CameraBoom"));
	//FAttachmentTransformRules Rule(EAttachmentRule::KeepRelative, true);
	//CameraBoom->AttachToComponent(GetOwner()->GetRootComponent(), Rule);
	////CameraBoom = ConstructObject<USpringArmComponent>(USpringArmComponent::StaticClass(), this, "CameraBoom");
	////CameraBoom->AttachTo(GetOwner()->GetRootComponent());
	//CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 120.0f);
	//CameraBoom->bDoCollisionTest = false;
	//CameraBoom->TargetArmLength = 500.0f;

	//CameraBoom->bUsePawnControlRotation = false;
	//CameraBoom->bInheritPitch = false;
	//CameraBoom->bInheritRoll = false;
	//CameraBoom->bInheritYaw = false;

	//Camera = NewObject<UCameraComponent>(GetOuter(), UCameraComponent::StaticClass(), TEXT("ObliqueViewCamera"));
	////Camera = ConstructObject<UCameraComponent>(UCameraComponent::StaticClass(), this, "ObliqueViewCamera");
	//Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//Camera->OrthoWidth = 2048.0f;
	//Camera->bAutoActivate = true;

	//CameraBoom->RelativeRotation = FRotator(CAMERA_ANGLE, 0.0f, 0.0f); // THIS ISN'T WORKING?????
	//CameraBoom->SetRelativeRotation(FRotator(CAMERA_ANGLE, 0.0f, 0.0f));
	//CameraBoom->AddLocalRotation(FRotator(/*CAMERA_ANGLE*/ 60, 0.0f, 0.0f));
}


// Called every frame
void UPlayerCameraControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCameraBounds(DeltaTime);


	//CameraBoom->AddLocalRotation(FRotator(CAMERA_ANGLE, 0.0f, 0.0f));
	// ...
}

void UPlayerCameraControllerComponent::SetCameraBounds(const UBoxComponent & CameraBoundsBox) {

	

	if (GetOwnerRole() == ROLE_Authority || GetOwnerRole() == ROLE_AutonomousProxy) {
		FVector BoxExtent = CameraBoundsBox.GetScaledBoxExtent();

		FVector Origin = CameraBoundsBox.GetComponentLocation();


		//UE_LOG(LogTemp, Warning, TEXT("Box Origin = %f , %f , %f"), Origin.X, Origin.Y, Origin.Z);

		CameraBoundsMin = FVector(Origin.X - BoxExtent.X, Origin.Y - BoxExtent.Y, Origin.Z - BoxExtent.Z);
		CameraBoundsMax = FVector(Origin.X + BoxExtent.X, Origin.Y + BoxExtent.Y, Origin.Z + BoxExtent.Z);

		//UE_LOG(LogTemp, Warning, TEXT("CameraBoundsMin = %f , %f , %f"), CameraBoundsMin.X, CameraBoundsMin.Y, CameraBoundsMin.Z);
	}
}

void UPlayerCameraControllerComponent::SetBoomReference(USpringArmComponent & NewSpringArm) {
	CameraBoom = &NewSpringArm;
	Camera = Cast<UCameraComponent>(CameraBoom->GetChildComponent(0));

}



void UPlayerCameraControllerComponent::UpdateCameraBounds(float DeltaTime) {
	if (GetOwnerRole() == ROLE_Authority || GetOwnerRole() == ROLE_AutonomousProxy) {
		if (CameraBoom != nullptr) {

			//if (CameraBoundsRef != nullptr) {
			FVector CameraLocation = CameraBoom->GetComponentLocation() ; //Camera->GetComponentLocation();
			FVector CameraVelocity = (GetOwner()->GetActorLocation() - CameraLocation);


			//UE_LOG(LogTemp, Warning, TEXT("GetActorForwardVector = %f , %f , %f"), GetOwner()->GetActorForwardVector().X, GetOwner()->GetActorForwardVector().Y, GetOwner()->GetActorForwardVector().Z);
			FVector Offset;

			if (GetOwner()->GetClass()->ImplementsInterface(USpriteCharacterInterface::StaticClass())) {
				Offset = ISpriteCharacterInterface::Execute_GetCharacterFaceDirection(GetOwner());
			}

			FVector ValueToClamp = CameraLocation + CameraVelocity + (Offset/*TODO: Get forward vector from character*/ * OffsetMagnitude) ;
			FVector MinimumClamp = CameraBoundsMin;
			FVector MaximumClamp = CameraBoundsMax;

			FVector TargetLocation = FVector(
				FMath::Clamp(ValueToClamp.X, CameraBoundsMin.X, CameraBoundsMax.X),
				FMath::Clamp(ValueToClamp.Y, CameraBoundsMin.Y, CameraBoundsMax.Y),
				FMath::Clamp(ValueToClamp.Z, CameraBoundsMin.Z, CameraBoundsMax.Z)
			);

			FVector CameraDestination = FMath::VInterpTo(CameraBoom->GetComponentLocation(), TargetLocation, DeltaTime, 3.5);

			CameraBoom->SetWorldLocation(CameraDestination);
		}
	}
}


