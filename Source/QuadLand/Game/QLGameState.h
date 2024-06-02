// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "QLGameState.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	AQLGameState();
	virtual void AddPlayerState(APlayerState* PlayerState) override;

	UFUNCTION()
	void GetWinner(const FGameplayTag CallbackTag, int32 NewCount);

protected:
	int LivePlayerCount; //PlayerDieStatus.Num °ªÀÌ¶û µ¿ÀÏ
	TMap<FName, bool> PlayerDieStatus;

};
