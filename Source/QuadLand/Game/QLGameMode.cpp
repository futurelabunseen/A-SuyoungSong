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
#include "Character/QLCharacterPlayer.h"
#include "EngineUtils.h"

AQLGameMode::AQLGameMode()
{

	static ConstructorHelpers::FClassFinder <AQLPlayerController> PlayerControllerClassRef(TEXT("/Game/QuadLand/Blueprints/BPQL_PlayerController.BPQL_PlayerController_C"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}

	PlayerStateClass = AQLPlayerState::StaticClass();

	GameStateClass = AQLGameState::StaticClass();

	LivePlayerCount = 0;
}


void AQLGameMode::SetPlayerDefaults(APawn* PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);

	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(PlayerPawn);

	if (Player)
	{
		AQLPlayerController* PC = Player->GetController<AQLPlayerController>();
		if (PC)
		{
			PC->ClientRPCGameStart();
		}
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


	AQLPlayerState* PS = PC->GetPlayerState<AQLPlayerState>();

	if (PS)
	{
		AddPlayer(FName(PS->GetName()));
	}
}

void AQLGameMode::SpawnAI()
{
	AQLAISpawner *AISpawner = GetWorld()->SpawnActor<AQLAISpawner>(AISpawnerClass);

	if (AISpawner)
	{
		AISpawner->SetLifeSpan(5.0f);
	}

	//GameState�� �����´�.
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
	//��� �÷��̾� State�� �����´�.
	//�÷��̾� State�� �±׸� Ȯ���Ѵ�.
	//Dead�� �����Ǿ��ִ� ��� �¸��ڿ��� �����Ѵ�.
	//�¸��ڰ� �Ѹ� ������ �� �¸��� �±׸� �����Ѵ�.
	//GetWorld�� �ִ� �÷��̾� ������Ʈ ��������


	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		AQLPlayerState* PlayerState = CastChecked<AQLPlayerState>(It->Get()->PlayerState);

		UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
		FName PlayerName = FName(PlayerState->GetName());
		//�±װ� �����Ǿ��ִ��� Ȯ���Ѵ�.
		if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_DEAD))
		{
			if (PlayerDieStatus[PlayerName] == false)
			{
				LivePlayerCount--; //ó�� ����
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

				//PlayerState�� ����ؼ� MulticastRPC ����

				FGameplayTagContainer TargetTag(CHARACTER_STATE_WIN);
				ASC->TryActivateAbilitiesByTag(TargetTag);
				//GameMode ���� �����ؾ���.

				//ServerRPC�� ����ؼ� �¸��� �÷��̾� ����
			}
			else
			{
				const auto PC = Cast<AQLPlayerController>(It->Get());
				PC->Loose();
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

void AQLGameMode::GameEnd() //������ ���������� ȣ�� �װų� or �¸��ϰų�
{

}


