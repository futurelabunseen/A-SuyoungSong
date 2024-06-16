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
#include "AttributeSet/QLAS_PlayerStat.h"
#include "Character/QLInventoryComponent.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Interface/QLAISpawnerInterface.h"
#include "Net/UnrealNetwork.h"
#include "GameData/QLDataManager.h"
#include "GameFramework/PlayerStart.h"
#include "Physics/QLCollision.h"
#include "EngineUtils.h"
#include "Game/QLGameInstance.h"
#include "GameFramework/GameModeBase.h"
#include "Game/QLGameMode.h"

AQLPlayerController::AQLPlayerController()
{
	bStartGame = false;
	bReadyGame = false;
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
		if (DeathTimerHandle.IsValid() == false)
		{
			CurrentDeathSec = Time;

			GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &AQLPlayerController::ReduceDeathSec, 1.0f, true);
		}
	}
	FTimerHandle StopTimer;
	GetWorld()->GetTimerManager().SetTimer(StopTimer, this, &AQLPlayerController::StopDeathSec, Time + 1.0f);
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
	SetVisibilityHUD(EHUDType::Win);
	UQLReturnToLobby* UserWidget = Cast<UQLReturnToLobby>(HUDs[EHUDType::Win]);

	if (UserWidget)
	{
		FInputModeUIOnly UIOnlyInputMode;
		SetInputMode(UIOnlyInputMode);
		SetShowMouseCursor(true);
		UserWidget->SetupUI();
	}
}

void AQLPlayerController::Loose()
{
	SetVisibilityHUD(EHUDType::Death);
	UQLReturnToLobby* UserWidget = Cast<UQLReturnToLobby>(HUDs[EHUDType::Death]);

	if (UserWidget)
	{
		FInputModeUIOnly UIOnlyInputMode;
		SetInputMode(UIOnlyInputMode);
		SetShowMouseCursor(true);
		UserWidget->SetupUI();
	}
}

void AQLPlayerController::CloseInventroy()
{
	UQLInventory* InventoryUI = Cast<UQLInventory>(HUDs[EHUDType::Inventory]);

	if (InventoryUI)
	{
		InventoryUI->RemoveAllNearbyItemEntries(); //���� ����
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
	UQLUserWidget* UserWidget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (UserWidget)
	{
		UserWidget->UpdateLivePlayer(InLivePlayer);
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

void AQLPlayerController::ServerRPCInitPawn_Implementation(int Type)
{
	AQLGameMode* GameMode = GetWorld()->GetAuthGameMode<AQLGameMode>();
	if (GameMode != nullptr)
	{
		GameMode->SpawnPlayerPawn(this, Type);
		ClientRPCCreateWidget();
	}
}

void AQLPlayerController::ClientRPCCreateWidget_Implementation()
{
	//�������� ������ �ϸ��
	if (HUDs.Find(EHUDType::HUD))
	{
		UQLUserWidget* Widget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

		AQLCharacterPlayer* QLCharacter = Cast<AQLCharacterPlayer>(GetPawn());
		if (Widget && QLCharacter)
		{
			QLCharacter->OnChangeShootingMethod.BindUObject(Widget, &UQLUserWidget::VisibleShootingMethodUI);
		}
	}

	//Map ��ġ�� �缱��
	if (HUDs.Find(EHUDType::Map))
	{
		UQLMap* Map = Cast<UQLMap>(HUDs[EHUDType::Map]);

		Map->ResetPlayer();
	}
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
	UQLDeathTimerWidget* DeathTimerWidget = Cast<UQLDeathTimerWidget>(HUDs[EHUDType::DeathTimer]);
	DeathTimerWidget->UpdateTimer(CurrentDeathSec);
	CurrentDeathSec--;
}

void AQLPlayerController::StopDeathSec()
{
	if (IsLocalController())
	{
		GetWorld()->GetTimerManager().ClearTimer(DeathTimerHandle);
		DeathTimerHandle.Invalidate();
	}
	//Delegateȣ��
	OnDeathCheckDelegate.ExecuteIfBound(); 
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
		Widget->ChangedRemainingAmmo(PS->GetMaxAmmoCnt()); //�ӽð� ����
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
	//SetHiddenHUD(EHUDType::Loading);
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

void AQLPlayerController::AddInventoryByDraggedItem(EItemType ItemIdx, int32 CurrentItemCnt)
{
	//Player����
	AQLCharacterPlayer* QLCharacter = Cast<AQLCharacterPlayer>(GetPawn());

	if (QLCharacter)
	{
		QLCharacter->GetInventory()->AddInventoryByDraggedItem(ItemIdx, CurrentItemCnt);
	}
}

void AQLPlayerController::RemoveItemEntry(EItemType ItemIdx)
{
	//Player����
	AQLCharacterPlayer* QLCharacter =Cast<AQLCharacterPlayer>(GetPawn());

	if (QLCharacter)
	{
		QLCharacter->GetInventory()->UseItem(ItemIdx);
	}

}

void AQLPlayerController::AddGroundByDraggedItem(EItemType ItemIdx, int32 CurrentItemCnt)
{
	//Player����
	AQLCharacterPlayer* QLCharacter = Cast<AQLCharacterPlayer>(GetPawn());

	if (QLCharacter)
	{
		QLCharacter->GetInventory()->AddGroundByDraggedItem(ItemIdx, CurrentItemCnt);
	}
}

void AQLPlayerController::ConcealLifeStone()
{
	UQLUserWidget* UserWidget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (UserWidget)
	{
		UserWidget->ConcealLifeStone();
	}
}
void AQLPlayerController::ServerRPCRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds(); //���� �����ð�

	ClientRPCReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AQLPlayerController::ClientRPCReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5*RoundTripTime); //�պ��ð��̴ϱ� /2

	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

//�÷��̾ ������ �ϸ�, �������� �ð��� ���� �� �ִ� ���� ���� �ð�.
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

						AQLCharacterPlayer* CharacterPlayer = GetPawn<AQLCharacterPlayer>();
						CharacterPlayer->ServerRPCInitNickname();
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

