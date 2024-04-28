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
	PlayerDieStatus.Add(FName(PlayerState->GetName()), false); //���������� üũ�� ����

	QL_LOG(QLNetLog, Log, TEXT("PlayerState Num = %d"), PlayerDieStatus.Num());


	for (const auto& PlayerStatus : PlayerDieStatus)
	{
		QL_LOG(QLNetLog, Log, TEXT("[Iterator] : Current Player %s %d"), *PlayerStatus.Key.ToString(), PlayerStatus.Value);
	}

	AQLPlayerState* NewPlayerState = CastChecked<AQLPlayerState>(PlayerState);
	UAbilitySystemComponent* ASC = NewPlayerState->GetAbilitySystemComponent();

	LivePlayerCount = PlayerArray.Num(); //���� ����ִ� �÷��̾� �� (�߰��ɶ����� ī��Ʈ)
	ASC->RegisterGameplayTagEvent(CHARACTER_STATE_DEAD, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLGameState::GetWinner);
}


void AQLGameState::GetWinner(const FGameplayTag CallbackTag, int32 NewCount)
{
	//��� �÷��̾� State�� �����´�.
	//�÷��̾� State�� �±׸� Ȯ���Ѵ�.
	//Dead�� �����Ǿ��ִ� ��� �¸��ڿ��� �����Ѵ�.
	//�¸��ڰ� �Ѹ� ������ �� �¸��� �±׸� �����Ѵ�.


	for (const auto& Player : PlayerArray)
	{

		AQLPlayerState* PlayerState = CastChecked<AQLPlayerState>(Player);

		UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
		FName PlayerName = FName(PlayerState->GetName());
		//�±װ� �����Ǿ��ִ��� Ȯ���Ѵ�.
		if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_DEAD))
		{
			if (PlayerDieStatus[PlayerName] == false)
			{
				LivePlayerCount--; //ó�� ����
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
				ASC->AddLooseGameplayTag(CHARACTER_STATE_WIN); //���Ŵ� RemoveLooseGameplayTag
			}
		}
	}

	
}
