// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/QLGameState.h"
#include "GameFramework/PlayerState.h"
#include "QuadLand.h"
#include "EngineUtils.h"
#include "GameplayTag/GamplayTags.h"
#include "AbilitySystemComponent.h"
#include "Player/QLPlayerState.h"
#include "GameData/QLItemDataset.h"
AQLGameState::AQLGameState()
{
	LivePlayerCount = 0;
}

void AQLGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	

	QL_LOG(QLNetLog, Log, TEXT("Current Player %s"), *PlayerState->GetName());
	PlayerDieStatus.Add(FName(PlayerState->GetName()), false); //서버에서만 체크할 예정

	QL_LOG(QLNetLog, Log, TEXT("PlayerState Num = %d"), PlayerDieStatus.Num());


	for (const auto& PlayerStatus : PlayerDieStatus)
	{
		QL_LOG(QLNetLog, Log, TEXT("[Iterator] : Current Player %s %d"), *PlayerStatus.Key.ToString(), PlayerStatus.Value);
	}

	AQLPlayerState* NewPlayerState = CastChecked<AQLPlayerState>(PlayerState);
	UAbilitySystemComponent* ASC = NewPlayerState->GetAbilitySystemComponent();

	LivePlayerCount = PlayerArray.Num(); //현재 살아있는 플레이어 수 (추가될때마다 카운트)
	ASC->RegisterGameplayTagEvent(CHARACTER_STATE_DEAD, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLGameState::GetWinner);
}


void AQLGameState::GetWinner(const FGameplayTag CallbackTag, int32 NewCount)
{
	//모든 플레이어 State를 가져온다.
	//플레이어 State의 태그를 확인한다.
	//Dead가 부착되어있는 경우 승리자에서 제외한다.
	//승리자가 한명 남았을 때 승리자 태그를 부착한다.


	for (const auto& Player : PlayerArray)
	{

		AQLPlayerState* PlayerState = CastChecked<AQLPlayerState>(Player);

		UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
		FName PlayerName = FName(PlayerState->GetName());
		//태그가 부착되어있는지 확인한다.
		if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_DEAD))
		{
			if (PlayerDieStatus[PlayerName] == false)
			{
				LivePlayerCount--; //처음 죽음
				QL_LOG(QLNetLog, Log, TEXT("Player Death %s"), *PlayerName.ToString());
			}
			PlayerDieStatus[PlayerName] = true;
		}
		//ASC->RegisterGameplayTagEvent(CHARACTER_STATE_DEAD, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLGameMode::GetWinner);
	}

	for (const auto& Player : PlayerArray)
	{

		AQLPlayerState* PlayerState = CastChecked<AQLPlayerState>(Player);
		UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
		FName PlayerName = FName(PlayerState->GetName());

		if (LivePlayerCount == 1)
		{
			if (PlayerDieStatus[PlayerName] == false)
			{
				QL_LOG(QLNetLog, Log, TEXT("Player Win %s"), *PlayerName.ToString());
				ASC->AddLooseGameplayTag(CHARACTER_STATE_WIN); //제거는 RemoveLooseGameplayTag
			}
		}
	}

	
}
