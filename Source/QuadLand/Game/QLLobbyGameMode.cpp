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
	TotalPlayers = 4; //�ʹݿ� 4��������, 10�� �̻� ���� �Ŀ��� ���� �����ϴ� �÷��̾� ���� Ȯ���ϰ� �ȴ�.
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

	//���� ������ ���� ���� �÷��̾� ��Ʈ�ѷ����� ���� �ð��� �ο� 
	
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
	//�ش� �÷��̾�� �ٸ� �÷��̾ ���������� UI ����
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
	//üũ���� ���� �÷��̾�� �ð� �ʸ� �����ִ� UI�� ����.
	//�ش� �÷��̾�� �ٸ� �÷��̾ ���������� UI ����
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
