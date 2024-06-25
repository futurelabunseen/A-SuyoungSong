// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/QLLobbyGameMode.h"
#include "GameFramework/GameState.h"
#include "Player/QLLobbyPlayerController.h"
#include "GameFramework/GameStateBase.h"
AQLLobbyGameMode::AQLLobbyGameMode()
{

	static ConstructorHelpers::FClassFinder <APlayerController> PlayerControllerClassRef(TEXT("/Game/QuadLand/Blueprints/BPQL_LobbyPlayerController.BPQL_LobbyPlayerController_C"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}

	bIsFirstCondition = false;
	ReadyPlayerNum = 0;
	TotalPlayers = 4; //초반엔 4명이지만, 10초 이상 지난 후에는 현재 존재하는 플레이어 수로 확정하게 된다.
}

void AQLLobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	
	if (NumberOfPlayers > 4)
	{
		ErrorMessage = TEXT("Exceed the number of participating players");
	}
}


void AQLLobbyGameMode::PostLogin(APlayerController* NewPC)
{
	Super::PostLogin(NewPC);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

	if (NumberOfPlayers >= 2)
	{
		GetWorld()->GetTimerManager().SetTimer(WaitTimeCheckTimer, this, &AQLLobbyGameMode::ConfirmPlayerCount, WaitTimeCheckTime, false);
	}
}

void AQLLobbyGameMode::ConfirmPlayerCount()
{
	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	
	TotalPlayers = NumberOfPlayers;

	if (ReadyPlayerNum == TotalPlayers)
	{
		GameStart();
	}

	GetWorld()->GetTimerManager().SetTimer(TimeLimitTimer, this, &AQLLobbyGameMode::ShowTimeLimit, LimitTime, false);

	//같지 않으면 레디 안한 플레이어 컨트롤러한테 제한 시간을 부여 
	
}

void AQLLobbyGameMode::GameStart()
{

	GetWorld()->GetTimerManager().ClearTimer(TimeLimitTimer);
	TimeLimitTimer.Invalidate();
	GetWorld()->GetTimerManager().ClearTimer(WaitTimeCheckTimer);
	WaitTimeCheckTimer.Invalidate();

	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel(FString("/Game/QuadLand/Maps/Main?listen"));
	}

}

void AQLLobbyGameMode::ReadyPlayer()
{
	if (ReadyPlayerNum < TotalPlayers)
	{
		ReadyPlayerNum++;
	}

	if (ReadyPlayerNum == TotalPlayers)
	{
		GameStart();
	}
	//해당 플레이어에게 다른 플레이어가 선택중임을 UI 띄우기
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		AQLLobbyPlayerController* PC = Cast<AQLLobbyPlayerController>(It->Get());

		if (PC->GetIsReady())
		{
			PC->ClientRPCShowWaitUI();
		}
	}
}

void AQLLobbyGameMode::ShowTimeLimit()
{
	//체크하지 않은 플레이어에게 시간 초를 보여주는 UI를 띄운다.
	//해당 플레이어에게 다른 플레이어가 선택중임을 UI 띄우기
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		AQLLobbyPlayerController* PC = Cast<AQLLobbyPlayerController>(It->Get());

		if (PC->GetIsReady() == false)
		{
			UE_LOG(LogTemp, Log, TEXT(" No Select %s"), *PC->GetName());
			PC->ClientRPCActivateTimeLimit();
		}
	}
}
