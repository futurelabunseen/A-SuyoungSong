// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/QLGameState.h"
#include "GameFramework/PlayerState.h"
#include "QuadLand.h"
#include "EngineUtils.h"
#include "GameplayTag/GamplayTags.h"
#include "AbilitySystemComponent.h"
#include "Player/QLPlayerState.h"
#include "GameFramework/Character.h"
#include "GameData/QLItemDataset.h"
#include "QLGameMode.h"

AQLGameState::AQLGameState()
{
	LivePlayerCount = 0;
}

void AQLGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	
	AQLPlayerState* NewPlayerState = Cast<AQLPlayerState>(PlayerState);
	if (NewPlayerState)
	{

		UAbilitySystemComponent* ASC = NewPlayerState->GetAbilitySystemComponent();
		if (ASC)
		{
			ASC->RegisterGameplayTagEvent(CHARACTER_STATE_DEAD, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLGameState::GetWinner);
		}
	}
}

void AQLGameState::GetWinner(const FGameplayTag CallbackTag, int32 NewCount)
{
	AQLGameMode* GameMode = Cast<AQLGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode)
	{
		GameMode->GetWinner(CallbackTag,NewCount);
	}
}
