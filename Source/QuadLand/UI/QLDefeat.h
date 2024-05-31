// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QLDefeat.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLDefeat : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void SetupDefeat();

	UFUNCTION()
	void OnDestorySession(bool bWasSuccessful);

	UFUNCTION()
	void ReturnButtonClicked();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ReturnLobbyButton;

	UPROPERTY()
	TObjectPtr<class UMultiplayerSessionsSubsystem> MultiplayerSessionSubsystem;

	UPROPERTY()
	TObjectPtr<class APlayerController> PlayerController;
};
