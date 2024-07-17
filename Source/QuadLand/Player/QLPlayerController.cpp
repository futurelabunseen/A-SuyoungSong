// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/QLPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h"
#include "Player/QLPlayerState.h"
#include "Character/QLCharacterPlayer.h"
#include "UI/QLPlayerHUDWidget.h"
#include "UI/QLInventory.h"
#include "UI/QLReturnToLobby.h"

#include "QuadLand.h"
#include "GameplayTag/GamplayTags.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "Character/QLInventoryComponent.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Interface/QLAISpawnerInterface.h"
#include "Net/UnrealNetwork.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Physics/QLCollision.h"
#include "EngineUtils.h"
#include "GameFramework/GameModeBase.h"
#include "Game/QLGameMode.h"
#include "GameFramework/PawnMovementComponent.h"
#include "HUD/QLHUD.h"

AQLPlayerController::AQLPlayerController()
{
	bStartGame = false;
	bReadyGame = false;
	bIsDanger = false;
}

void AQLPlayerController::SetHiddenHUD(EHUDType UItype)
{
	if (IsLocalController() && HUDs.Find(UItype))
	{
		HUDs[UItype]->SetVisibility(ESlateVisibility::Hidden);
	}
}
void AQLPlayerController::SetVisibilityHUD(EHUDType UItype)
{
	if (IsLocalController() && HUDs.Find(UItype))
	{
		HUDs[UItype]->SetVisibility(ESlateVisibility::Visible);
	}
}

void AQLPlayerController::CloseHUD(EHUDType UItype)
{	
	FInputModeGameOnly GameOnlyInputMode;
	SetHiddenHUD(UItype);
	bShowMouseCursor = false;
	SetInputMode(GameOnlyInputMode);
}
void AQLPlayerController::ActivateDeathTimer(float Time)
{
	if (IsLocalController())
	{
		AQLHUD* LocalHUD = Cast<AQLHUD>(GetHUD());
		if (LocalHUD)
		{
			LocalHUD->SetVisibilityHUD(EHUDType::DeathTimer);
		}
	}

	if (DeathTimerHandle.IsValid() == false)
	{
		bIsDanger = true;
		CurrentDeathSec = 10.0f;
	}
}
void AQLPlayerController::Win()
{
	if (const auto PlayerController = Cast<APlayerController>(this))
	{
		if (const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
		}
	}

	SetVisibilityHUD(EHUDType::Win);
	UQLReturnToLobby* UserWidget = Cast<UQLReturnToLobby>(HUDs[EHUDType::Win]);

	if (UserWidget)
	{
		FInputModeUIOnly UIOnlyInputMode;
		SetInputMode(UIOnlyInputMode);
		SetShowMouseCursor(true);
		AQLPlayerState* PS = GetPlayerState<AQLPlayerState>();

		if (PS)
		{
			UserWidget->SettingTxtPhase(PS->GetPlayerName(), ChangeTimeText());
		}
		UserWidget->SetupUI();
	}
}

void AQLPlayerController::ClientRPCLoose_Implementation()
{
	if (const auto PlayerController = Cast<APlayerController>(this))
	{
		if (const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
		}
	}

	if (HUDs.Find(EHUDType::Death) == 0)
	{
		return;
	}

	SetVisibilityHUD(EHUDType::Death);
	UQLReturnToLobby* UserWidget = Cast<UQLReturnToLobby>(HUDs[EHUDType::Death]);

	if (UserWidget)
	{
		FInputModeUIOnly UIOnlyInputMode;
		SetInputMode(UIOnlyInputMode);
		SetShowMouseCursor(true);
		AQLPlayerState* PS = GetPlayerState<AQLPlayerState>();
		APawn *TmpPawn = GetPawn();

		if (TmpPawn)
		{
			TmpPawn->GetMovementComponent()->StopActiveMovement();
		}

		if (PS)
		{
			if (DeathTime.IsEmpty())
			{
				DeathTime = ChangeTimeText();
			}

			UserWidget->SettingTxtPhase(PS->GetPlayerName(), DeathTime);
		}
		UserWidget->SetupUI();
	}
}

void AQLPlayerController::CloseInventroy()
{
	UQLInventory* InventoryUI = Cast<UQLInventory>(HUDs[EHUDType::Inventory]);

	if (InventoryUI)
	{
		CloseHUD(EHUDType::Inventory);
	}
}

void AQLPlayerController::SetUpdateLivePlayer(int16 InLivePlayer)
{
	AQLHUD* LocalHUD = Cast<AQLHUD>(GetHUD());

	if (LocalHUD)
	{
		LocalHUD->SetUpdateLivePlayer(InLivePlayer);
	}
}

void AQLPlayerController::SettingDeathTime()
{
	DeathTime = ChangeTimeText(); //Update
}

void AQLPlayerController::RemoveAllNearbyItemEntries()
{
	UQLInventory* InventoryUI = Cast<UQLInventory>(HUDs[EHUDType::Inventory]);

	if (InventoryUI)
	{
		InventoryUI->RemoveAllNearbyItemEntries(); //전부 제거
	}

}

FString AQLPlayerController::ChangeTimeText()
{
	uint32 ProgressTime = GetServerTime() - StartTime;

	int32 Minutes = FMath::FloorToInt(ProgressTime / 60.f);
	int32 Seconds = ProgressTime - Minutes * 60;

	FString ProgressTimeText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	
	return ProgressTimeText;
}

void AQLPlayerController::ResetUI()
{
	if (HUDs.Num() == 0) return;
	UQLInventory* InventoryUI = Cast<UQLInventory>(HUDs[EHUDType::Inventory]);
	if (InventoryUI)
	{
		InventoryUI->ClearAll();
	}

	AQLHUD* LocalHUD = Cast<AQLHUD>(GetHUD());

	if (LocalHUD)
	{
		LocalHUD->ResetUI();
	}
}

void AQLPlayerController::CloseAllUI()
{
	for (const auto HUD : HUDs)
	{
		HUD.Value->SetVisibility(ESlateVisibility::Hidden);
	}

	AQLHUD* LocalHUD = Cast<AQLHUD>(GetHUD());

	if (LocalHUD)
	{
		LocalHUD->CloseAllUI();
	}
}


void AQLPlayerController::ServerRPCInitPawn_Implementation(int Type)
{
	AQLGameMode* GameMode = GetWorld()->GetAuthGameMode<AQLGameMode>();
	if (GameMode != nullptr)
	{
		GameMode->SpawnPlayerPawn(this, Type);
		ClientRPCCreateWidget();
		AQLCharacterPlayer* QLCharacter = Cast<AQLCharacterPlayer>(GetPawn());

		if (QLCharacter)
		{
			QLCharacter->ServerRPCInitNickname(); //닉네임 결정
		}
	}
}

void AQLPlayerController::InitWidget()
{
	if (HUDs.Find(EHUDType::Win))
	{
		UQLReturnToLobby* Win = Cast<UQLReturnToLobby>(HUDs[EHUDType::Win]);

		if (Win)
		{
			Win->SetupUI();
		}
	}

	if (HUDs.Find(EHUDType::Death))
	{
		UQLReturnToLobby* Loose = Cast<UQLReturnToLobby>(HUDs[EHUDType::Death]);

		if (Loose)
		{
			Loose->SetupUI();
		}
	}

	AQLHUD* LocalHUD = Cast<AQLHUD>(GetHUD());

	if (LocalHUD)
	{
		LocalHUD->InitWidget();
	}
}
void AQLPlayerController::ClientRPCCreateWidget_Implementation()
{
	InitWidget();
}


void AQLPlayerController::ClientRPCUpdateLivePlayer_Implementation(int16 InLivePlayer)
{
	SetUpdateLivePlayer(InLivePlayer);
}

void AQLPlayerController::ClientRPCGameStart_Implementation()
{
	bReadyGame = true;
}

void AQLPlayerController::ReduceDeathSec()
{
	int32 SecondsLeft = FMath::CeilToInt(CurrentDeathSec - ElapsedTime);
	AQLHUD* LocalHUD = Cast<AQLHUD>(GetHUD());

	if (LocalHUD)
	{
		LocalHUD->ReduceDeathSec(SecondsLeft);
	}
}


void AQLPlayerController::StopDeathSec()
{
	if (IsLocalController())
	{
		AQLHUD* LocalHUD = Cast<AQLHUD>(GetHUD());

		if (LocalHUD)
		{
			LocalHUD->ReduceDeathSec(0);
		}
	}
	//Delegate호출

	if (OnDeathCheckDelegate.IsBound())
	{
		OnDeathCheckDelegate.Execute();
	}
}


void AQLPlayerController::ClientRPCShowLifestoneWidget_Implementation(float Timer)
{
	AQLHUD* LocalHUD = Cast<AQLHUD>(GetHUD());

	if (LocalHUD)
	{
		LocalHUD->ShowStoneUI(Timer);
	}
}

void AQLPlayerController::CreateHUD()
{
	if (HUDClass.Num() == 0) return;

	if (!IsLocalPlayerController())
	{
		return;
	}
	
	for (const auto &HUD : HUDClass)
	{
		UUserWidget *Widget = CreateWidget<UUserWidget>(GetWorld(), HUD.Value);
		Widget->AddToViewport(5);
		Widget->SetVisibility(ESlateVisibility::Visible);
		HUDs.Add(HUD.Key, Widget);
	}

	SetHiddenHUD(EHUDType::Story);
	SetHiddenHUD(EHUDType::Inventory);
	SetHiddenHUD(EHUDType::Menu);
	SetHiddenHUD(EHUDType::Win);
	SetHiddenHUD(EHUDType::Death);

}
void AQLPlayerController::UpdateNearbyItemEntry(UObject* Item)
{
	if (IsLocalController())
	{
		UQLInventory* InventoryUI = Cast<UQLInventory>(HUDs[EHUDType::Inventory]);
		if (InventoryUI)
		{
			InventoryUI->UpdateNearbyItemEntry(Item);
		}
	}
}

void AQLPlayerController::UpdateItemEntry(UObject* Item, int32 CurrentItemCnt)
{
	if (IsLocalController())
	{
		UQLInventory* InventoryUI = Cast<UQLInventory>(HUDs[EHUDType::Inventory]);
		if (InventoryUI)
		{
			InventoryUI->UpdateItemEntry(Item, CurrentItemCnt);
		}
	}
}

void AQLPlayerController::AddInventoryByDraggedItem(EItemType ItemIdx)
{
	//Player전달
	AQLCharacterPlayer* QLCharacter = Cast<AQLCharacterPlayer>(GetPawn());

	if (QLCharacter)
	{
		QLCharacter->GetInventory()->AddInventoryByDraggedItem(ItemIdx);
	}
}

void AQLPlayerController::RemoveItemEntry(EItemType ItemIdx)
{
	//Player전달
	AQLCharacterPlayer* QLCharacter =Cast<AQLCharacterPlayer>(GetPawn());

	if (QLCharacter)
	{
		QLCharacter->GetInventory()->UseItem(ItemIdx);
	}

}

void AQLPlayerController::AddGroundByDraggedItem(EItemType ItemIdx)
{
	//Player전달
	AQLCharacterPlayer* QLCharacter = Cast<AQLCharacterPlayer>(GetPawn());

	if (QLCharacter)
	{
		QLCharacter->GetInventory()->AddGroundByDraggedItem(ItemIdx);
	}
}


void AQLPlayerController::ServerRPCRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds(); //현재 받은시간

	ClientRPCReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AQLPlayerController::ClientRPCReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5*RoundTripTime); //왕복시간이니까 /2

	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

//플레이어를 재정의 하면, 서버에서 시간을 얻을 수 있는 가장 빠른 시간.
void AQLPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRPCRequestServerTime(GetWorld()->GetTimeSeconds());
	}

}
void AQLPlayerController::ServerTimeCheck(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;

	if (IsLocalController() && TimeSyncFrequency < TimeSyncRunningTime)
	{
		ServerRPCRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.0f;
	}
}

void AQLPlayerController::UpdateProgressTime()
{
	uint32 ProgressTime = GetServerTime() - StartTime;

	int32 Minutes = FMath::FloorToInt(ProgressTime / 60.f);
	int32 Seconds = ProgressTime - Minutes * 60;

	FString ProgressTimeText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	AQLHUD* LocalHUD = Cast<AQLHUD>(GetHUD());
	if (LocalHUD)
	{
		LocalHUD->UpdateProgressTime(ProgressTimeText);
	}

}

float AQLPlayerController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}

	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}
void AQLPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bReadyGame)
	{
		SetHUDTime();
	}

	if (bIsDanger)
	{
		ElapsedTime += DeltaTime;
		ReduceDeathSec();
		if (ElapsedTime >= CheckInterval)
		{
			bIsDanger = false;
			if (OnDeathCheckDelegate.IsBound())
			{
				OnDeathCheckDelegate.Execute();
			}
		}
	}
	
	UpdateProgressTime();
	ServerTimeCheck(DeltaTime);
}

void AQLPlayerController::SetHUDTime()
{
	//10 
	if (bStartGame == false)
	{
		int32 SecondsLeft = FMath::CeilToInt(MatchTime - GetServerTime());

		if (SecondsLeft >= 0 && CountDownInt != SecondsLeft)
		{
			AQLHUD* LocalHUD = GetHUD<AQLHUD>();

			if (LocalHUD)
			{
				if (IsLocalController())
				{
					LocalHUD->UpdateLoadingSec(SecondsLeft);
				}
				if (SecondsLeft == 0)
				{
					IQLAISpawnerInterface* AISpawnerInterface = GetWorld()->GetAuthGameMode<IQLAISpawnerInterface>();

					if (AISpawnerInterface)
					{
						AISpawnerInterface->SpawnAI();
					}
					LocalHUD->SetHiddenHUD(EHUDType::Loading);
					SetVisibilityHUD(EHUDType::Story);
					bStartGame = true;
					StartTime = GetServerTime();

				}
			}
		}
		CountDownInt = SecondsLeft;
	}
}

void AQLPlayerController::ClientRPCOutLobby_Implementation()
{
	// 클라이언트에게 세션 종료 알림을 보냅니다.
//	ClientReturnToMainMenuWithTextReason(FText());
	
	GetWorld()->ServerTravel(FString("/Game/QuadLand/Maps/Opening?listen"));
}

void AQLPlayerController::PlayCanSound()
{
	AQLCharacterPlayer* TmpCharacter = Cast<AQLCharacterPlayer>(GetPawn());

	if (TmpCharacter)
	{
		TmpCharacter->PlayCanSound();
	}
}

void AQLPlayerController::RemoveNearbyItemEntry(UObject *Item)
{
	if (IsLocalController())
	{
		UQLInventory* InventoryUI = Cast<UQLInventory>(HUDs[EHUDType::Inventory]);
		if (InventoryUI)
		{
			InventoryUI->RemoveNearbyItemEntry(Item);
		}
	}
}
