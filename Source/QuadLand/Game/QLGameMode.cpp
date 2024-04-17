// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/QLGameMode.h"
#include "QuadLand.h"
#include "Player/QLPlayerController.h"
#include "Player/QLPlayerState.h"
#include "AbilitySystemComponent.h"
#include "EngineUtils.h"
#include "GameplayTag/GamplayTags.h"
AQLGameMode::AQLGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Game/QuadLand/Blueprints/BPQL_Player.BPQL_Player_C"));

	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder <APlayerController> PlayerControllerClassRef(TEXT("/Game/QuadLand/Blueprints/BPQL_PlayerController.BPQL_PlayerController_C"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
	
	PlayerStateClass = AQLPlayerState::StaticClass();
	PlayerCnt = 1;;
	
}

void AQLGameMode::StartGame()
{
	//��ư�� ������ �� ������ �÷��̾ Ȯ���ϰ� �����ϴ� �ɷ� ����.
	QL_LOG(QLNetLog, Log, TEXT("Current Position"));
	for (AQLPlayerState* PlayerStart : TActorRange<AQLPlayerState>(GetWorld()))
	{
		UAbilitySystemComponent *ASC = PlayerStart->GetAbilitySystemComponent();
		ASC->RegisterGameplayTagEvent(CHARACTER_STATE_DEAD, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLGameMode::GetWinner);

		FName PlayerName = FName(PlayerStart->GetName());
		PlayerDieStatus[PlayerName] = false;
		PlayerCnt++;
	}
}

void AQLGameMode::StartPlay()
{
	Super::StartPlay();
}

void AQLGameMode::GetWinner(const FGameplayTag CallbackTag, int32 NewCount)
{
	//��� �÷��̾� State�� �����´�.
	//�÷��̾� State�� �±׸� Ȯ���Ѵ�.
	//Dead�� �����Ǿ��ִ� ��� �¸��ڿ��� �����Ѵ�.
	//�¸��ڰ� �Ѹ� ������ �� �¸��� �±׸� �����Ѵ�.

	for (AQLPlayerState* PlayerStart : TActorRange<AQLPlayerState>(GetWorld()))
	{
		UAbilitySystemComponent* ASC = PlayerStart->GetAbilitySystemComponent();

		//�±װ� �����Ǿ��ִ��� Ȯ���Ѵ�.
		if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_DEAD))
		{
			FName PlayerName = FName(PlayerStart->GetName());
			QL_LOG(QLNetLog, Log, TEXT("Player Death %s"), *PlayerName.ToString());
			PlayerDieStatus[PlayerName] = true;
			PlayerCnt--; //������ ī��Ʈ ����.
		}
		//ASC->RegisterGameplayTagEvent(CHARACTER_STATE_DEAD, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLGameMode::GetWinner);
	}

	if (PlayerCnt == 1)
	{
		//�Ѹ� ��Ƴ���
		//������ Ȯ��
		for (AQLPlayerState* PlayerStart : TActorRange<AQLPlayerState>(GetWorld()))
		{
			FName PlayerName = FName(PlayerStart->GetName());
			if (PlayerDieStatus[PlayerName] == false)
			{
				QL_LOG(QLNetLog, Log, TEXT("Player Win %s"), *PlayerName.ToString());
				UAbilitySystemComponent* ASC = PlayerStart->GetAbilitySystemComponent();
				ASC->AddLooseGameplayTag(CHARACTER_STATE_WIN); //���Ŵ� RemoveLooseGameplayTag
			}
		}
	}
}
