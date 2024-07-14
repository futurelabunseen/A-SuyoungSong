// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/QLPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h"
#include "Player/QLPlayerState.h"
#include "Character/QLCharacterPlayer.h"
#include "UI/QLPlayerHUDWidget.h"
#include "UI/QLUserWidget.h"
#include "UI/QLInventory.h"
#include "UI/QLMap.h"
#include "UI/QLBloodWidget.h"
#include "UI/QLReturnToLobby.h"
#include "UI/QLDeathTimerWidget.h"
#include "UI/QLLoadingPanel.h"
#include "QuadLand.h"
#include "GameplayTag/GamplayTags.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "Character/QLInventoryComponent.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Interface/QLAISpawnerInterface.h"
#include "Net/UnrealNetwork.h"
#include "GameData/QLDataManager.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Physics/QLCollision.h"
#include "EngineUtils.h"
#include "Game/QLGameInstance.h"
#include "GameFramework/GameModeBase.h"
#include "Game/QLGameMode.h"
#include "GameFramework/PawnMovementComponent.h"

AQLPlayerController::AQLPlayerController()
{
	bStartGame = false;
	bReadyGame = false;
	bIsDanger = false;
}

void AQLPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDs.Num() == 0)
	{
		CreateHUD();
	}
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

void AQLPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (IsLocalController()&&HUDs.Num() == 0)
	{
		CreateHUD();
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
		SetVisibilityHUD(EHUDType::DeathTimer);
	}

	if (DeathTimerHandle.IsValid() == false)
	{
		bIsDanger = true;
		CurrentDeathSec = 10.0f;
		//GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &AQLPlayerController::ReduceDeathSec, 1.0f, true);
	}

	//FTimerHandle StopTimer;
	//GetWorld()->GetTimerManager().SetTimer(StopTimer, this, &AQLPlayerController::StopDeathSec, Time + 1.0f);
}

void AQLPlayerController::BlinkBloodWidget()
{
	SetVisibilityHUD(EHUDType::Blood);
	UQLBloodWidget* Widget = Cast<UQLBloodWidget>(HUDs[EHUDType::Blood]);

	if (Widget)
	{
		Widget->BlinkWidget();
	}
	FTimerHandle CancelTimer;

	GetWorld()->GetTimerManager().SetTimer(CancelTimer, this,&AQLPlayerController::CancelBloodWidget, 3.0f, false);

}

void AQLPlayerController::CancelBloodWidget()
{
	UQLBloodWidget* Widget = Cast<UQLBloodWidget>(HUDs[EHUDType::Blood]);

	if (Widget)
	{
		Widget->CancelWidget();
	}
	SetHiddenHUD(EHUDType::Blood);

}

void AQLPlayerController::BlinkBag()
{
	UQLUserWidget* UserWidget = Cast< UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (UserWidget)
	{
		UserWidget->BlinkBag();
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
		//InventoryUI->RemoveAllNearbyItemEntries(); //전부 제거
		CloseHUD(EHUDType::Inventory);
	}
}

void AQLPlayerController::SwitchWeaponStyle(ECharacterAttackType AttackType)
{
	UQLUserWidget* UserWidget = Cast< UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (UserWidget)
	{
		UserWidget->SwitchWeaponStyle(AttackType);
	}
}

void AQLPlayerController::SetUpdateLivePlayer(int16 InLivePlayer)
{
	if (HUDs.Find(EHUDType::HUD) == 0)
	{
		return;
	}

	UQLUserWidget* UserWidget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (UserWidget)
	{
		UserWidget->UpdateLivePlayer(InLivePlayer);
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

void AQLPlayerController::SettingNickname()
{
	AQLPlayerState* PS = GetPlayerState<AQLPlayerState>();

	if (PS)
	{
		UQLUserWidget* UserWidget = Cast< UQLUserWidget>(HUDs[EHUDType::HUD]);
		UserWidget->SettingNickname(PS->GetPlayerName());
	}
}

void AQLPlayerController::InitWidget()
{	//내생각에 서버만 하면됨
	if (HUDs.Find(EHUDType::HUD))
	{
		UQLUserWidget* Widget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

		AQLCharacterPlayer* QLCharacter = Cast<AQLCharacterPlayer>(GetPawn());
		if (Widget && QLCharacter)
		{
			QLCharacter->OnChangeShootingMethod.BindUObject(Widget, &UQLUserWidget::VisibleShootingMethodUI);
		}
	}

	//Map 위치도 재선정
	if (HUDs.Find(EHUDType::Map))
	{
		UQLMap* Map = Cast<UQLMap>(HUDs[EHUDType::Map]);

		Map->ResetPlayer();
	}

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

	UQLUserWidget* UserWidget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);
	if (UserWidget)
	{
		UserWidget->UpdateEquipWeaponUI(false);
		UserWidget->UpdateEquipBombUI(false);
	}
}

void AQLPlayerController::CloseAllUI()
{
	for (const auto HUD : HUDs)
	{
		HUD.Value->SetVisibility(ESlateVisibility::Hidden);
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

void AQLPlayerController::ClientRPCCreateWidget_Implementation()
{
	InitWidget();
}


void AQLPlayerController::InitStoneTexture(int GemType)
{
	UQLUserWidget* UserWidget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);
	UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());

	if (UserWidget)
	{
		UserWidget->SettingStoneImg(DataManager->GemTexture(GemType));
	}
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
	if (HUDs.Find(EHUDType::DeathTimer) == 0)
	{
		bIsDanger = false;
		return;
	}

	int32 SecondsLeft = FMath::CeilToInt(CurrentDeathSec - ElapsedTime);

	if (IsLocalController())
	{
		UQLDeathTimerWidget* DeathTimerWidget = Cast<UQLDeathTimerWidget>(HUDs[EHUDType::DeathTimer]);
		DeathTimerWidget->UpdateTimer(SecondsLeft);
	}
	
	//CurrentDeathSec--;
}

void AQLPlayerController::StopDeathSec()
{
	if (IsLocalController())
	{

		UQLDeathTimerWidget* DeathTimerWidget = Cast<UQLDeathTimerWidget>(HUDs[EHUDType::DeathTimer]);
		DeathTimerWidget->UpdateTimer(0);
	}
	//Delegate호출
	
	if (OnDeathCheckDelegate.IsBound())
	{
		OnDeathCheckDelegate.Execute();
	}
}


void AQLPlayerController::ClientRPCShowLifestoneWidget_Implementation(float Timer)
{
	UQLMap *Map = Cast<UQLMap>(HUDs[EHUDType::Map]);

	if (Map)
	{
		Map->ShowLifestoneBox(Timer);
	}
}

void AQLPlayerController::CreateHUD()
{
	if (HUDClass.Num() == 0) return;

	if (!IsLocalPlayerController())
	{
		return;
	}
	
	AQLPlayerState* PS = GetPlayerState<AQLPlayerState>();
	if (!PS)
	{
		QL_LOG(QLNetLog, Warning, TEXT("PlayerState is not founded"));
		return;
	}

	for (const auto &HUD : HUDClass)
	{

		UUserWidget *Widget = CreateWidget<UUserWidget>(this, HUD.Value);
		
		if (EHUDType::Blood == HUD.Key)
		{
			Widget->AddToViewport();
		}
		else
		{
			Widget->AddToViewport(5);
		}
		Widget->SetVisibility(ESlateVisibility::Visible);
		HUDs.Add(HUD.Key, Widget);
	}

	UQLUserWidget *Widget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (Widget)
	{
		Widget->ChangedAmmoCnt(PS->GetCurrentAmmoCnt());
		Widget->ChangedRemainingAmmo(PS->GetMaxAmmoCnt()); //임시값 삽입
		Widget->ChangedHPPercentage(PS->GetHealth(), PS->GetMaxHealth());
		Widget->ChangedStaminaPercentage(PS->GetStamina(), PS->GetMaxStamina());
		SettingNickname();
	}

	SetHiddenHUD(EHUDType::Inventory);
	SetHiddenHUD(EHUDType::Map);
	SetHiddenHUD(EHUDType::DeathTimer);
	SetHiddenHUD(EHUDType::Blood);
	SetHiddenHUD(EHUDType::Menu);
	SetHiddenHUD(EHUDType::KeyGuide);
	SetHiddenHUD(EHUDType::Win);
	SetHiddenHUD(EHUDType::Death);
	UQLGameInstance* GameInstance = Cast<UQLGameInstance>(GetWorld()->GetGameInstance());

	if (GameInstance)
	{
		InitStoneTexture(GameInstance->GetGemMatType());
	}

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


void AQLPlayerController::UpdateEquipWeaponUI(bool InVisible)
{
	UQLUserWidget* UserWidget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);
	
	if (UserWidget)
	{
		UserWidget->UpdateEquipWeaponUI(InVisible);
	}
}

void AQLPlayerController::UpdateEquipBombUI(bool InVisible)
{
	UQLUserWidget* UserWidget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (UserWidget)
	{
		UserWidget->UpdateEquipBombUI(InVisible);
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

void AQLPlayerController::AddGroundByDraggedItem(EItemType ItemIdx, int32 CurrentItemCnt)
{
	//Player전달
	AQLCharacterPlayer* QLCharacter = Cast<AQLCharacterPlayer>(GetPawn());

	if (QLCharacter)
	{
		QLCharacter->GetInventory()->AddGroundByDraggedItem(ItemIdx, CurrentItemCnt);
	}
}

void AQLPlayerController::ConcealLifeStone(bool InVisible)
{
	UQLUserWidget* UserWidget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (UserWidget)
	{
		UserWidget->ConcealLifeStone(InVisible);
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
	if (HUDs.Find(EHUDType::HUD) == 0) return;

	UQLUserWidget* UserWidget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (UserWidget)
	{
		uint32 ProgressTime = GetServerTime() - StartTime;

		int32 Minutes = FMath::FloorToInt(ProgressTime / 60.f);
		int32 Seconds = ProgressTime - Minutes * 60;

		FString ProgressTimeText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		UserWidget->UpdateProgressTime(ProgressTimeText);
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
			if (HUDs.Find(EHUDType::Loading) != nullptr)
			{
				UQLLoadingPanel* UserWidget = Cast<UQLLoadingPanel>(HUDs[EHUDType::Loading]);
				if (UserWidget)
				{
					UserWidget->SetTxtRemainingTime(SecondsLeft);

					if (SecondsLeft == 0)
					{
						IQLAISpawnerInterface* AISpawnerInterface = GetWorld()->GetAuthGameMode<IQLAISpawnerInterface>();
						
						if (AISpawnerInterface)
						{
							AISpawnerInterface->SpawnAI();
						}
						if (HUDs.Find(EHUDType::Loading))
						{
							SetHiddenHUD(EHUDType::Loading);
						}
						bStartGame = true;
						StartTime = GetServerTime();
					}
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
