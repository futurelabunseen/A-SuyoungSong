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
#include "GameData/QLDataManager.h"
#include "Game/QLGameInstance.h"
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

	QL_LOG(QLLog, Log, TEXT("begin"));

	AQLPlayerState* NewPlayerState = NewPlayer->GetPlayerState<AQLPlayerState>();
	if (NewPlayerState)
	{
		UAbilitySystemComponent* ASC = NewPlayerState->GetAbilitySystemComponent();
		if (ASC)
		{
			ASC->RegisterGameplayTagEvent(CHARACTER_STATE_DEAD, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLGameMode::GetWinner);
		}
	}

}

void AQLGameMode::SpawnAI()
{

	AQLAISpawner *AISpawner = GetWorld()->SpawnActor<AQLAISpawner>(AISpawnerClass);

	if (AISpawner)
	{
		AISpawner->SetLifeSpan(5.0f);
	}

}

void AQLGameMode::DeadNonPlayer(FName NonPlayerName)
{

	if (PlayerDieStatus.Find(NonPlayerName) && PlayerDieStatus[NonPlayerName] == false)
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
			}
			PlayerDieStatus[PlayerName] = true;
		}

		const auto PC = Cast<AQLPlayerController>(It->Get());
		PC->ClientRPCUpdateLivePlayer(LivePlayerCount);
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
				//PlayerState를 사용해서 MulticastRPC 전송
				//GameMode 에게 전달해야함
				//ServerRPC를 사용해서 승리자 플레이어 전달
				FGameplayTagContainer TargetTag(CHARACTER_STATE_WIN);
				ASC->TryActivateAbilitiesByTag(TargetTag);
			}
			else
			{
				const auto PC = Cast<AQLPlayerController>(It->Get());
				PC->ClientRPCLoose();
			}
		}
	}
}

void AQLGameMode::AddPlayer(FName PlayerName)
{
	PlayerDieStatus.Add(PlayerName, false);
	LivePlayerCount++;
}

void AQLGameMode::SpawnPlayerPawn(APlayerController* Player,int Type)
{
	if (Player == nullptr)
		return;

	// 현재 Pawn을 가져옵니다.
	APawn* OldPawn = Player->GetPawn();
	if (OldPawn != nullptr)
	{
		// 기존 Pawn 제거
		OldPawn->Destroy();
		Player->UnPossess(); // 기존의 소유 상태 해제
	}

	// 새로운 Pawn 생성
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = Player;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor *StartSpot = ChoosePlayerStart(Player);

	if (StartSpot)
	{
		UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());
	
		APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(DataManager->GetSkeletalMesh(Type), StartSpot->GetActorLocation(),StartSpot->GetActorRotation(), SpawnInfo);

		if (ResultPawn)
		{
			Player->Possess(ResultPawn);
			Player->ClientRestart(ResultPawn);
		}
	}

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


