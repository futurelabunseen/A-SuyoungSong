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
		if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_DEAD) && ( PlayerState->GetHasLifeStone() || ASC->HasMatchingGameplayTag(CHARACTER_STATE_DANGER)))
		{
			if (PlayerDieStatus[PlayerName] == false)
			{
				LivePlayerCount--; //ó�� ����
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
				//PlayerState�� ����ؼ� MulticastRPC ����
				//GameMode ���� �����ؾ���
				//ServerRPC�� ����ؼ� �¸��� �÷��̾� ����
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

void AQLGameMode::SpawnPlayerPawn(APlayerController* PC,int Type)
{
	if (PC == nullptr)
		return;

	// ���� Pawn�� �����ɴϴ�.
	APawn* OldPawn = PC->GetPawn();
	if (OldPawn != nullptr)
	{
		// ���� Pawn ����
		OldPawn->Destroy();
		PC->UnPossess(); // ������ ���� ���� ����
	}

	// ���ο� Pawn ����
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = PC;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor *StartSpot = ChoosePlayerStart(PC);

	if (StartSpot)
	{
		UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());
	
		AQLCharacterPlayer* ResultPawn = GetWorld()->SpawnActor<AQLCharacterPlayer>(DataManager->GetSkeletalMesh(Type), StartSpot->GetActorLocation(),StartSpot->GetActorRotation(), SpawnInfo);

		if (ResultPawn)
		{
			PC->Possess(ResultPawn);
			PC->ClientRestart(ResultPawn);
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

void AQLGameMode::GameEnd() //������ ���������� ȣ�� �װų� or �¸��ϰų�
{

}


