// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "QLGameMode.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AQLGameMode();
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	/** Called after a successful login.  This is the first place it is safe to call replicated functions on the PlayerController. */
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:

	void GameStart();

};
