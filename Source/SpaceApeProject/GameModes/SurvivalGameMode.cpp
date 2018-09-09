// Fill out your copyright notice in the Description page of Project Settings.

#include "SurvivalGameMode.h"
#include "SpriteObjects/PlayerPaperCharacter.h"
#include "WorldObjects/BaseRoom.h"
//#include"Components/RoomComponents/BaseSpawnComponent.h"
#include "Spawning/BaseSpawn.h"
#include "SpriteObjects/PlayerPaperCharacter.h"

// Copied from GameModeBase TODO: Confirm which includes are required and delete the rest
#include "GameFramework/GameNetworkManager.h"
#include "Matinee/MatineeActor.h"
#include "Engine/LevelScriptActor.h"
#include "Engine/World.h"
#include "Misc/CommandLine.h"
#include "UObject/Package.h"
#include "Misc/PackageName.h"
#include "Net/OnlineEngineInterface.h"
#include "GameFramework/GameStateBase.h"
#include "PhysicsEngine/BodyInstance.h"
#include "GameFramework/DefaultPawn.h"
#include "GameFramework/SpectatorPawn.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/WorldSettings.h"
#include "Engine/NetConnection.h"
#include "Engine/ChildConnection.h"
#include "Engine/PlayerStartPIE.h"
#include "Engine/LocalPlayer.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreaming.h"

#include "Engine/World.h"
#include "Engine/Public/EngineUtils.h"
#include "Kismet/GameplayStatics.h"

ASurvivalGameMode::ASurvivalGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnOb(TEXT("/Game/CharacterBlueprints/PlayerPaperCharacter_BP"));
	DefaultPawnClass = PlayerPawnOb.Class;

	/*static ConstructorHelpers::FClassFinder<APawn> PlayerPawnOb(TEXT("/Game/Blueprints/Pawns/PlayerPawn"));
	DefaultPawnClass = PlayerPawnOb.Class;

	static ConstructorHelpers::FClassFinder<APawn> BotPawnOb(TEXT("/Game/Blueprints/Pawns/BotPawn"));
	BotPawnClass = BotPawnOb.Class;

	HUDClass = AShooterHUD::StaticClass();
	PlayerControllerClass = AShooterPlayerController::StaticClass();
	PlayerStateClass = AShooterPlayerState::StaticClass();
	SpectatorClass = AShooterSpectatorPawn::StaticClass();
	GameStateClass = AShooterGameState::StaticClass();
	ReplaySpectatorPlayerControllerClass = AShooterDemoSpectator::StaticClass();

	MinRespawnDelay = 5.0f;

	bAllowBots = true;
	bNeedsBotCreation = true;
	bUseSeamlessTravel = true;*/
}

//void ASurvivalGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) {
	//const int32 BotsCountOptionValue = UGameplayStatics::GetIntOption(Options, GetBotsCountOptionName(), 0);
	//SetAllowBots(BotsCountOptionValue > 0 ? true : false, BotsCountOptionValue);
	//Super::InitGame(MapName, Options, ErrorMessage);

	//const UGameInstance* GameInstance = GetGameInstance();
	//if (GameInstance && Cast<UShooterGameInstance>(GameInstance)->GetOnlineMode() != EOnlineMode::Offline)
	//{
	//	bPauseable = false;
	//}
//}

void ASurvivalGameMode::PreInitializeComponents() {
	Super::PreInitializeComponents();

	//GetWorldTimerManager().SetTimer(TimerHandle_DefaultTimer, this, &ASurvivalGameMode::DefaultTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

//void ASurvivalGameMode::DefaultTimer() {
//	// don't update timers for Play In Editor mode, it's not real match
//	if (GetWorld()->IsPlayInEditor())
//	{
//		// start match if necessary.
//		if (GetMatchState() == MatchState::WaitingToStart)
//		{
//			StartMatch();
//		}
//		return;
//	}
//
//	AShooterGameState* const MyGameState = Cast<AShooterGameState>(GameState);
//	if (MyGameState && MyGameState->RemainingTime > 0 && !MyGameState->bTimerPaused)
//	{
//		MyGameState->RemainingTime--;
//
//		if (MyGameState->RemainingTime <= 0)
//		{
//			if (GetMatchState() == MatchState::WaitingPostMatch)
//			{
//				RestartGame();
//			}
//			else if (GetMatchState() == MatchState::InProgress)
//			{
//				FinishMatch();
//
//				// Send end round events
//				for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
//				{
//					AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(*It);
//
//					if (PlayerController && MyGameState)
//					{
//						AShooterPlayerState* PlayerState = Cast<AShooterPlayerState>((*It)->PlayerState);
//						const bool bIsWinner = IsWinner(PlayerState);
//
//						PlayerController->ClientSendRoundEndEvent(bIsWinner, MyGameState->ElapsedTime);
//					}
//				}
//			}
//			else if (GetMatchState() == MatchState::WaitingToStart)
//			{
//				StartMatch();
//			}
//		}
//	}
//}


//void ASurvivalGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) {
//	//AShooterGameState* const MyGameState = Cast<AShooterGameState>(GameState);
//	//const bool bMatchIsOver = MyGameState && MyGameState->HasMatchEnded();
//	//if (bMatchIsOver)
//	//{
//	//	ErrorMessage = TEXT("Match is over!");
//	//}
//	//else
//	//{
//		// GameSession can be NULL if the match is over
//		Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
//	//}
//}


//void ASurvivalGameMode::PostLogin(APlayerController* NewPlayer) {
//	Super::PostLogin(NewPlayer);
//
//	// update spectator location for client
//	APlayerController* NewPC = Cast<APlayerController>(NewPlayer);
//	if (NewPC && NewPC->GetPawn() == NULL) {
//		//NewPC->ClientSetSpectatorCamera(NewPC->GetSpawnLocation(), NewPC->GetControlRotation());
//	}
//
//	//// notify new player if match is already in progress
//	//if (NewPC && IsMatchInProgress())
//	//{
//	//	NewPC->ClientGameStarted();
//	//	NewPC->ClientStartOnlineGame();
//	//}
//}


AActor * ASurvivalGameMode::ChoosePlayerStart_Implementation(AController * Player) {
	TArray<ABaseSpawn*> FoundPlayerSpawns;

	ABaseSpawn* BestStart = NULL;
	for (TActorIterator<ABaseRoom> It(GetWorld()); It; ++It) {

		ABaseRoom* FoundRoom = *It;

		if (FoundRoom->IsA<ABaseRoom>()) {
			TArray<AActor*> ChildActors;

			FoundRoom->GetAllChildActors(ChildActors);

			if (ChildActors.Num() > 0) {

				//UE_LOG(LogTemp, Warning, TEXT("SpawnCount for room It = %d"), SpawnComponents.Num());


				for (AActor* Child : ChildActors) {

					if (Child->IsA(ABaseSpawn::StaticClass())) {
						if (Cast<ABaseSpawn>(Child)->IsPlayerSpawn) {
							FoundPlayerSpawns.Add(Cast<ABaseSpawn>(Child));
						}
					}
				}
			}
		}
	}

	if (FoundPlayerSpawns.Num() > 0) {
		UE_LOG(LogTemp, Warning, TEXT("FoundPlayerSpawns count = %d"), FoundPlayerSpawns.Num());
		BestStart = FoundPlayerSpawns[0];
	}

	if (!BestStart) {
		UE_LOG(LogTemp, Warning, TEXT("BestStart is null"));

		
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("BestStart "));
	}
	return BestStart ? Cast<AActor>(BestStart) : Super::ChoosePlayerStart_Implementation(Player);
}



//FString ASurvivalGameMode::InitNewPlayer(APlayerController * NewPlayerController, const FUniqueNetIdRepl & UniqueId, const FString & Options, const FString & Portal) {
//
//	UE_LOG(LogTemp, Warning, TEXT("ASurvivalGameMode::InitNewPlayer"));
//
//	//PRAGMA_DISABLE_DEPRECATION_WARNINGS
//	//	// Try calling deprecated version first
//	//	FString DeprecatedError = InitNewPlayer(NewPlayerController, UniqueId.GetUniqueNetId(), Options, Portal);
//	//if (DeprecatedError != TEXT("DEPRECATED")) {
//	//	// This means it was implemented in subclass
//	//	return DeprecatedError;
//	//}
//	//PRAGMA_ENABLE_DEPRECATION_WARNINGS
//
//	check(NewPlayerController);
//
//	FString ErrorMessage;
//
//	// Register the player with the session
//	GameSession->RegisterPlayer(NewPlayerController, UniqueId.GetUniqueNetId(), UGameplayStatics::HasOption(Options, TEXT("bIsFromInvite")));
//
//	// Find a starting spot
//	AActor* const StartSpot = FindPlayerStart(NewPlayerController, Portal);
//	if (StartSpot != nullptr) {
//		// Set the player controller / camera in this new location
//		FRotator InitialControllerRot = StartSpot->GetActorRotation();
//		InitialControllerRot.Roll = 0.f;
//		NewPlayerController->SetInitialLocationAndRotation(StartSpot->GetActorLocation(), InitialControllerRot);
//		NewPlayerController->StartSpot = StartSpot;
//
//		if (ABaseSpawn* SpawnComponent = Cast<ABaseSpawn>(StartSpot)) {
//			SpawnComponent->SpawnPlayerCharacter(NewPlayerController);
//		}
//	}
//	else {
//		ErrorMessage = FString::Printf(TEXT("Failed to find PlayerStart"));
//	}
//
//	// Set up spectating
//	bool bSpectator = FCString::Stricmp(*UGameplayStatics::ParseOption(Options, TEXT("SpectatorOnly")), TEXT("1")) == 0;
//	if (bSpectator || MustSpectate(NewPlayerController)) {
//		NewPlayerController->StartSpectatingOnly();
//	}
//
//	// Init player's name
//	FString InName = UGameplayStatics::ParseOption(Options, TEXT("Name")).Left(20);
//	if (InName.IsEmpty())
//	{
//		InName = FString::Printf(TEXT("%s%i"), *DefaultPlayerName.ToString(), NewPlayerController->PlayerState->PlayerId);
//	}
//
//	ChangeName(NewPlayerController, InName, false);
//
//	return ErrorMessage;
//}

void ASurvivalGameMode::RestartPlayer(AController * NewPlayer) {
	UE_LOG(LogTemp, Warning, TEXT("ASurvivalGameMode::RestartPlayer"));
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
	{
		return;
	}

	AActor* StartSpot = FindPlayerStart(NewPlayer);



	// If a start spot wasn't found,
	if (StartSpot == nullptr)
	{
		// Check for a previously assigned spot
		if (NewPlayer->StartSpot != nullptr)
		{
			StartSpot = NewPlayer->StartSpot.Get();
			UE_LOG(LogGameMode, Warning, TEXT("RestartPlayer: Player start not found, using last start spot"));
		}
	}

	RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
}



//void ASurvivalGameMode::RestartPlayer(AController * NewPlayer) {
//	UE_LOG(LogTemp, Warning, TEXT("ASurvivalGameMode::RestartPlayer"));
//
//	FVector SpawnOrigin = FVector::ZeroVector;
//	FRotator StartRotation = FRotator::ZeroRotator;
//
//	TArray<ABaseSpawn*> FoundPlayerSpawns;
//
//	ABaseSpawn* BestStart = NULL;
//	for (TActorIterator<ABaseRoom> It(GetWorld()); It; ++It) {
//
//
//		ABaseRoom* FoundRoom = *It;
//
//		if (FoundRoom->IsA<ABaseRoom>()) {
//			TArray<AActor*> ChildActors;
//
//			FoundRoom->GetAllChildActors(ChildActors);
//
//			if (ChildActors.Num() > 0) {
//				for (AActor* Child : ChildActors) {
//					if (Child->IsA(ABaseSpawn::StaticClass())) {
//						if (Cast<ABaseSpawn>(Child)->IsPlayerSpawn ) {
//							FoundPlayerSpawns.Add( Cast<ABaseSpawn>(Child) );
//						}
//					}
//				}
//			}
//		}
//	}
//
//	if (FoundPlayerSpawns.Num() > 0) {
//		SpawnOrigin = FoundPlayerSpawns[0]->GetActorLocation();
//		StartRotation = FoundPlayerSpawns[0]->GetActorRotation();
//
//		FoundPlayerSpawns[0]->SpawnPlayerCharacter(NewPlayer);
//	}
//
//	//TArray<AActor*> FoundActors;
//	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseSpawn::StaticClass(), FoundActors);
//
//
//	//for (AActor* Actor : FoundActors) {
//	//	UE_LOG(LogTemp, Warning, TEXT("Number of spawns found = %d"), FoundActors.Num());
//
//	//	ABaseSpawn* SpawnPoint = Cast<ABaseSpawn>(Actor);
//
//	//	if (SpawnPoint && SpawnPoint->DefaultSpawnType == APlayerPaperCharacter::StaticClass()) {
//	//	}
//	//	else FoundActors.Remove(Actor);
//	//}
//
//	//if (FoundActors.Num() > 0) {
//	//	SpawnOrigin = FoundActors[0]->GetActorLocation();
//	//	StartRotation = FoundActors[0]->GetActorRotation();
//	//}
//
//	/* No player is alive (yet) - spawn using one of the PlayerStarts */ 
//	//if (SpawnOrigin == FVector::ZeroVector) {
//	//	Super::RestartPlayer(NewPlayer); 
//	//	return; 
//	//}
//
//
//
//	// Try to create a pawn to use of the default class for this player if (NewPlayer->GetPawn() == nullptr && GetDefaultPawnClassForController(NewPlayer) != nullptr) { FActorSpawnParameters SpawnInfo; SpawnInfo.Instigator = Instigator; APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(GetDefaultPawnClassForController(NewPlayer), StartLocation, StartRotation, SpawnInfo); if (ResultPawn == nullptr) { UE_LOG(LogGameMode, Warning, TEXT("Couldn't spawn Pawn of type %s at %s"), *GetNameSafe(DefaultPawnClass), &StartLocation); } NewPlayer->SetPawn(ResultPawn); }
//	//if (NewPlayer->GetPawn() == nullptr) { NewPlayer->FailedToSpawnPawn(); }
//	//else {
//	//	NewPlayer->Possess(NewPlayer->GetPawn());
//
//	//	// If the Pawn is destroyed as part of possession we have to abort if (NewPlayer->GetPawn() == nullptr) { NewPlayer->FailedToSpawnPawn(); } else { // Set initial control rotation to player start's rotation NewPlayer->ClientSetRotation(NewPlayer->GetPawn()->GetActorRotation(), true);
//
//	//	FRotator NewControllerRot = StartRotation; NewControllerRot.Roll = 0.f; NewPlayer->SetControlRotation(NewControllerRot);
//
//	//	SetPlayerDefaults(NewPlayer->GetPawn());
//	//}
//
//	if (BestStart) RestartPlayerAtPlayerStart(NewPlayer, BestStart);
//}
