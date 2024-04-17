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
	//버튼을 눌렀을 때 입장한 플레이어를 확인하고 저장하는 걸로 시작.
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
	//모든 플레이어 State를 가져온다.
	//플레이어 State의 태그를 확인한다.
	//Dead가 부착되어있는 경우 승리자에서 제외한다.
	//승리자가 한명 남았을 때 승리자 태그를 부착한다.

	for (AQLPlayerState* PlayerStart : TActorRange<AQLPlayerState>(GetWorld()))
	{
		UAbilitySystemComponent* ASC = PlayerStart->GetAbilitySystemComponent();

		//태그가 부착되어있는지 확인한다.
		if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_DEAD))
		{
			FName PlayerName = FName(PlayerStart->GetName());
			QL_LOG(QLNetLog, Log, TEXT("Player Death %s"), *PlayerName.ToString());
			PlayerDieStatus[PlayerName] = true;
			PlayerCnt--; //죽으면 카운트 없앰.
		}
		//ASC->RegisterGameplayTagEvent(CHARACTER_STATE_DEAD, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLGameMode::GetWinner);
	}

	if (PlayerCnt == 1)
	{
		//한명만 살아남음
		//누군지 확인
		for (AQLPlayerState* PlayerStart : TActorRange<AQLPlayerState>(GetWorld()))
		{
			FName PlayerName = FName(PlayerStart->GetName());
			if (PlayerDieStatus[PlayerName] == false)
			{
				QL_LOG(QLNetLog, Log, TEXT("Player Win %s"), *PlayerName.ToString());
				UAbilitySystemComponent* ASC = PlayerStart->GetAbilitySystemComponent();
				ASC->AddLooseGameplayTag(CHARACTER_STATE_WIN); //제거는 RemoveLooseGameplayTag
			}
		}
	}
}
