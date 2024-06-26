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

	void DestoryServer();
	UFUNCTION()
	void ReturnButtonClicked();

	UFUNCTION()
	void SettingTxtPhase(const FString& Nickname, const FString& Time);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Standard)
	FString DelimiterNickname;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Standard)
	FString DelimiterTime;
private:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ReturnLobbyButton;
	UPROPERTY()
	TObjectPtr<class APlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<class UMultiplayerSessionsSubsystem> MultiplayerSessionSubsystem;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TxtPhrase;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TxtTime;
};
