// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SurvivalGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SPACEAPEPROJECT_API ASurvivalGameMode : public AGameModeBase
{
	GENERATED_BODY()

	virtual void PreInitializeComponents() override;

	/** Initialize the game. This is called before actors' PreInitializeComponents. */
	//virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	/** Accept or reject a player attempting to join the server.  Fails login if you set the ErrorMessage to a non-empty string. */
	//virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	/** starts match warmup */
	//virtual void PostLogin(APlayerController* NewPlayer) override;

	/** select best spawn point for player */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/**
	* Customize incoming player based on URL options
	*
	* @param NewPlayerController player logging in
	* @param UniqueId unique id for this player
	* @param Options URL options that came at login
	*
	*/
	//virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;

	///** always pick new random spawn */
	//virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

	/** returns default pawn class for given controller */
	//virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	///** prevents friendly fire */
	//virtual float ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

	///** notify about kills */
	//virtual void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);

	///** can players damage each other? */
	//virtual bool CanDealDamage(AShooterPlayerState* DamageInstigator, AShooterPlayerState* DamagedPlayer) const;

	///** always create cheat manager */
	//virtual bool AllowCheats(APlayerController* P) override;

	///** update remaining time */ 
	//virtual void DefaultTimer();

	///** called before startmatch */
	//virtual void HandleMatchIsWaitingToStart() override;

	///** starts new match */
	//virtual void HandleMatchHasStarted() override;

	///** hides the onscreen hud and restarts the map */
	//virtual void RestartGame() override;

	///** Creates AIControllers for all bots */
	//void CreateBotControllers();

	///** Create a bot */
	//AShooterAIController* CreateBot(int32 BotNum);

	
public:
	ASurvivalGameMode(const FObjectInitializer& ObjectInitializer);
	
	virtual void RestartPlayer(class AController* NewPlayer) override;
};
