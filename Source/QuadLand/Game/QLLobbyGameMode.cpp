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
}


void AQLLobbyGameMode::PostLogin(APlayerController* NewPC)
{
	Super::PostLogin(NewPC);
}

void AQLLobbyGameMode::GameStart()
{

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