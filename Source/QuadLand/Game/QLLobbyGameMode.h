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

	virtual void PostLogin(APlayerController* NewPC) override;

	void GameStart();
	FTimerHandle StartTimerHandle;

	virtual void Tick(float DeltaSeconds) override;


	uint8 bIsFirstCondition : 1;
};
