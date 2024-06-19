// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "QLLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLLobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AQLLobbyGameMode();

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage);

	virtual void PostLogin(APlayerController* NewPC) override;

	void ConfirmPlayerCount();
	void GameStart();

	void ReadyPlayer();

	void ShowTimeLimit();

protected:

	FTimerHandle WaitTimeCheckTimer;
	FTimerHandle TimeLimitTimer;

	uint8 bIsFirstCondition : 1;

	int8 ReadyPlayerNum;
	int8 TotalPlayers;



	UPROPERTY(EditAnywhere, Category = Time)
	int8 WaitTimeCheckTime;
	UPROPERTY(EditAnywhere, Category = Time)
	int8 LimitTime;


};
