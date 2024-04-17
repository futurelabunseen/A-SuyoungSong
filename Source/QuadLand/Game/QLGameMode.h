// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "QLGameMode.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AQLGameMode();

	UFUNCTION()
	void GetWinner(const FGameplayTag CallbackTag, int32 NewCount);
	UFUNCTION(BlueprintCallable)
	void StartGame();
	virtual void StartPlay() override;
protected:

	int PlayerCnt;

	TMap<FName, bool> PlayerDieStatus;
};
