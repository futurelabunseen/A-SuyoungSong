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

	FTimerHandle StartTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(StartTimerHandle, this, &AQLGameMode::GameStart, 1.0f, false);
}

void AQLGameMode::StartPlay()
{
	Super::StartPlay();
}

void AQLGameMode::SpawnAI()
{
	AQLAISpawner *AISpawner = GetWorld()->SpawnActor<AQLAISpawner>(AISpawnerClass);

	if (AISpawner)
	{
		AISpawner->SetLifeSpan(5.0f);
	}
	QL_LOG(QLNetLog, Log, TEXT("Player %d "), LivePlayerCount);

	for (const auto& Players : PlayerDieStatus)
	{
		const auto PC = Cast<AQLPlayerController>(Players.Key);

		if (PC)
		{
			PC->ClientRPCUpdateLivePlayer(LivePlayerCount);
		}
	}
	//GameState를 가져온다.
}

void AQLGameMode::DeadNonPlayer(ACharacter* NonPlayerName)
{
		
	if (PlayerDieStatus.Find(NonPlayerName)&&PlayerDieStatus[NonPlayerName] == false)
	{
		PlayerDieStatus[NonPlayerName] = true;
		LivePlayerCount--;
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


	for (const auto& Players : PlayerDieStatus)
	{
		AQLCharacterBase* Character = Cast<AQLCharacterBase>(Players.Key);

		if (Character)
		{
			UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
			if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_DEAD))
			{
				if (Players.Value == false)
				{
					LivePlayerCount--; //처음 죽음
					const auto PC = Character->GetController<AQLPlayerController>();
					if (PC)
					{
						PC->ClientRPCUpdateLivePlayer(LivePlayerCount);
						QL_LOG(QLNetLog, Log, TEXT("Player Death %s"), *Character->GetName());
					}
					PlayerDieStatus[Character] = true;
				}
			}
		}
	}


	for (const auto& Players : PlayerDieStatus)
	{
		AQLCharacterBase* Character = Cast<AQLCharacterBase>(Players.Key);

		if (Character)
		{
			UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();

			if (LivePlayerCount == 1)
			{
				
				if (ASC && Players.Value == false)
				{
					FGameplayTagContainer TargetTag(CHARACTER_STATE_WIN);
					ASC->TryActivateAbilitiesByTag(TargetTag);
				}
			}
			const auto PC = Character->GetController<AQLPlayerController>();
			if (ASC && ASC->HasMatchingGameplayTag(CHARACTER_STATE_DEAD))
			{
				if (PC && PC->IsLocalController())
				{
					PC->Loose();
				}
			}
		}
	}

}

void AQLGameMode::AddPlayer(ACharacter* Player)
{
	PlayerDieStatus.Add(Player, false);
	RealPlayer.Add(Player);
	LivePlayerCount++;
}

ACharacter* AQLGameMode::NextCharacter(int32 NextIndex)
{

	if (LivePlayerCount == 1) return nullptr; //승리 결정해야함.

	AQLCharacterBase* Character = Cast<AQLCharacterBase>(RealPlayer[NextIndex]);
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();

	while(ASC->HasMatchingGameplayTag(CHARACTER_STATE_DEAD) == false)
	{
		NextIndex = (NextIndex + 1) % RealPlayer.Num();
	}
	
	return RealPlayer[NextIndex];
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


