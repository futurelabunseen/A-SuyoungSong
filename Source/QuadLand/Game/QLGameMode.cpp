// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/QLGameMode.h"
#include "QuadLand.h"
#include "Player/QLPlayerController.h"
#include "Player/QLPlayerState.h"
#include "Game/QLGameState.h"
#include "Gimmick/QLAISpawner.h"
#include "QuadLand.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/GamplayTags.h"
#include "GameFramework/Character.h"
#include "Character/QLCharacterNonPlayer.h"
#include "EngineUtils.h"

AQLGameMode::AQLGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Game/QuadLand/Blueprints/BPQL_Player.BPQL_Player_C"));

	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder <AQLPlayerController> PlayerControllerClassRef(TEXT("/Game/QuadLand/Blueprints/BPQL_PlayerController.BPQL_PlayerController_C"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}

	PlayerStateClass = AQLPlayerState::StaticClass();

	GameStateClass = AQLGameState::StaticClass();

	LivePlayerCount = 0;
}

void AQLGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	if (GetNumPlayers() >= 4)
	{
		ErrorMessage = TEXT("All players are participating.");
		return;
	}
}

void AQLGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	AQLPlayerController* PC = Cast<AQLPlayerController>(NewPlayer);

	if (PC == nullptr)
	{
		QL_LOG(QLNetLog, Error, TEXT("PlayerController Error %s"), *PC->GetName());
		return;
	}

	if (GetNumPlayers() >= 4) 
	{
		PC->bReadyGame = true;
	}

}

void AQLGameMode::StartPlay()
{
	Super::StartPlay();

	FTimerHandle StartTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(StartTimerHandle, this, &AQLGameMode::GameStart, 3.0f, false);

}

void AQLGameMode::SpawnAI()
{
	AQLAISpawner *AISpawner = GetWorld()->SpawnActor<AQLAISpawner>(AISpawnerClass);

	if (AISpawner)
	{
		AISpawner->SetLifeSpan(5.0f);
	}
	QL_LOG(QLNetLog, Log, TEXT("Player %d "), LivePlayerCount);

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		const auto PC = Cast<AQLPlayerController>(It->Get());

		PC->ClientRPCUpdateLivePlayer(LivePlayerCount);
	}
	//GameState를 가져온다.
}

void AQLGameMode::DeadNonPlayer(FName NonPlayerName)
{

	if (PlayerDieStatus.Find(NonPlayerName) && PlayerDieStatus[NonPlayerName] == false)
	{
		PlayerDieStatus[NonPlayerName] = true;
		LivePlayerCount--;

		QL_LOG(QLNetLog, Log, TEXT("Non Player Death %s"), *NonPlayerName.ToString());
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		const auto PC = Cast<AQLPlayerController>(It->Get());

		PC->ClientRPCUpdateLivePlayer(LivePlayerCount);
	}
}


void AQLGameMode::GetWinner(const FGameplayTag CallbackTag, int32 NewCount)
{
	//모든 플레이어 State를 가져온다.
	//플레이어 State의 태그를 확인한다.
	//Dead가 부착되어있는 경우 승리자에서 제외한다.
	//승리자가 한명 남았을 때 승리자 태그를 부착한다.
	//GetWorld에 있는 플레이어 스테이트 가져오기


	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		AQLPlayerState* PlayerState = CastChecked<AQLPlayerState>(It->Get()->PlayerState);

		UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
		FName PlayerName = FName(PlayerState->GetName());
		//태그가 부착되어있는지 확인한다.
		if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_DEAD))
		{
			if (PlayerDieStatus[PlayerName] == false)
			{
				LivePlayerCount--; //처음 죽음
				const auto PC = Cast<AQLPlayerController>(It->Get());
				PC->ClientRPCUpdateLivePlayer(LivePlayerCount);
				QL_LOG(QLNetLog, Log, TEXT("Player Death %s"), *PlayerName.ToString());
			}
			PlayerDieStatus[PlayerName] = true;
		}

	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		AQLPlayerState* PlayerState = CastChecked<AQLPlayerState>(It->Get()->PlayerState);
		UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
		FName PlayerName = FName(PlayerState->GetName());

		if (LivePlayerCount == 1)
		{
			if (PlayerDieStatus[PlayerName] == false)
			{
				QL_LOG(QLNetLog, Log, TEXT("Player Win %s"), *PlayerName.ToString());

				//PlayerState를 사용해서 MulticastRPC 전송

				FGameplayTagContainer TargetTag(CHARACTER_STATE_WIN);
				ASC->TryActivateAbilitiesByTag(TargetTag);
				//GameMode 에게 전달해야함.

				//ServerRPC를 사용해서 승리자 플레이어 전달
			}
		}
	}


}

void AQLGameMode::AddPlayer(FName PlayerName)
{
	PlayerDieStatus.Add(PlayerName, false);
	LivePlayerCount++;
}

void AQLGameMode::GameStart()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		const auto PC = Cast<AQLPlayerController>(It->Get());

		if (PC)
		{
			PC->ClientRPCGameStart();
		}
	}
}

void AQLGameMode::GameEnd() //게임이 끝날때마다 호출 죽거나 or 승리하거나
{

}


