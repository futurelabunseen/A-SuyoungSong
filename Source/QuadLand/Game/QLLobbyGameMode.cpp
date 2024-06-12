// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/QLLobbyGameMode.h"
#include "GameFramework/GameState.h"
#include "Player/QLLobbyPlayerController.h"
AQLLobbyGameMode::AQLLobbyGameMode()
{

	static ConstructorHelpers::FClassFinder <APlayerController> PlayerControllerClassRef(TEXT("/Game/QuadLand/Blueprints/BPQL_LobbyPlayerController.BPQL_LobbyPlayerController_C"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}

	bIsFirstCondition = false;
}


void AQLLobbyGameMode::PostLogin(APlayerController* NewPC)
{
	Super::PostLogin(NewPC);
	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

	if (NumberOfPlayers >= 2)
	{
		GetWorld()->GetTimerManager().ClearTimer(StartTimerHandle);
		StartTimerHandle.Invalidate();
	//	GetWorld()->GetTimerManager().SetTimer(StartTimerHandle, this, &AQLLobbyGameMode::GameStart, 3.0f, false);
	}
	

	//if (StartTimerHandle.IsValid() == false)
	//{
	//	UE_LOG(LogTemp, Log, TEXT("1"));
	//	GetWorld()->GetTimerManager().SetTimer(StartTimerHandle, this, &AQLLobbyGameMode::GameStart, 5.0f, false);
	//}
}

void AQLLobbyGameMode::GameStart()
{

	UE_LOG(LogTemp, Log, TEXT("2"));
	UWorld* World = GetWorld();

	if (World)
	{
		World->ServerTravel(FString("/Game/QuadLand/Maps/Main?listen"));
	}

}

void AQLLobbyGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsFirstCondition == false)
	{
		bool bIsCheck = true;
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
		{
			AQLLobbyPlayerController* PC = Cast<AQLLobbyPlayerController>(It->Get());

			if (PC->GetIsReady() == false)
			{
				bIsCheck = false;
			}
		}

		if (bIsCheck)
		{
			//바로 시작안하고 로딩 시간 넣을 예정.
			GameStart();
			bIsFirstCondition = true;
		}
	}
}
