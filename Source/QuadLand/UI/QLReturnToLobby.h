// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QLReturnToLobby.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLReturnToLobby : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void SetupUI();

	UFUNCTION()
	void OnDestorySession(bool bWasSuccessful);

	UFUNCTION()
	void ReturnButtonClicked();

	UFUNCTION()
	void SettingTxtPhase(const FString& Nickname);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Standard)
	FString Delimiter;
private:

	virtual void NativeConstruct() override;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ReturnLobbyButton;

	UPROPERTY()
	TObjectPtr<class UMultiplayerSessionsSubsystem> MultiplayerSessionSubsystem;

	UPROPERTY()
	TObjectPtr<class APlayerController> PlayerController;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TxtPhrase;


};
