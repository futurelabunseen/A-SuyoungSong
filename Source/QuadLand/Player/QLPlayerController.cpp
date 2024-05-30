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
#include "UI/QLDeathTimerWidget.h"
#include "QuadLand.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "Character/QLInventoryComponent.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

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
		SetVisibilityHUD(EHUDType::DeathTimer);
		if (DeathTimerHandle.IsValid() == false)
		{
			CurrentDeathSec = Time;
			QL_LOG(QLNetLog, Warning, TEXT("this? %d"), CurrentDeathSec);

			GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, this, &AQLPlayerController::ReduceDeathSec, 1.0f, true, 0.0f);
		}
	}
	FTimerHandle StopTimer;
	GetWorld()->GetTimerManager().SetTimer(StopTimer, this, &AQLPlayerController::StopDeathSec, Time, true, -1.0f);
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

void AQLPlayerController::CloseInventroy()
{
	UQLInventory* InventoryUI = Cast<UQLInventory>(HUDs[EHUDType::Inventory]);

	if (InventoryUI)
	{
		InventoryUI->RemoveAllNearbyItemEntries(); //전부 제거
		CloseHUD(EHUDType::Inventory);
	}
}

void AQLPlayerController::SwitchWeaponStyle(ECharacterAttackType AttackType)
{
	UQLUserWidget* UserWidget = Cast< UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (UserWidget)
	{
		QL_LOG(QLNetLog, Warning, TEXT("Current Attack Type %d"), AttackType);
		UserWidget->SwitchWeaponStyle(AttackType);
	}
}

void AQLPlayerController::ClientRPCGameStart_Implementation()
{
	SetHiddenHUD(EHUDType::Loading);
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
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
	//Delegate호출
	OnDeathCheckDelegate.ExecuteIfBound(); 
}

void AQLPlayerController::ClientRPCShowLifestoneWidget_Implementation(float Timer)
{
	UQLMap *Map = Cast<UQLMap>(HUDs[EHUDType::Map]);

	if (Map)
	{
		QL_LOG(QLNetLog, Warning, TEXT("this?"));
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
		Widget->AddToViewport();
		Widget->SetVisibility(ESlateVisibility::Visible);
		HUDs.Add(HUD.Key, Widget);
	}

	UQLUserWidget *Widget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

	Widget->ChangedAmmoCnt(PS->GetCurrentAmmoCnt());
	Widget->ChangedRemainingAmmo(PS->GetMaxAmmoCnt()); //임시값 삽입
	Widget->ChangedHPPercentage(PS->GetHealth(), PS->GetMaxHealth());
	Widget->ChangedStaminaPercentage(PS->GetStamina(), PS->GetMaxStamina());
	
	SetHiddenHUD(EHUDType::Inventory);
	SetHiddenHUD(EHUDType::Map);
	SetHiddenHUD(EHUDType::DeathTimer);
	SetHiddenHUD(EHUDType::Loading);
	SetHiddenHUD(EHUDType::Blood);

//	FInputModeUIOnly UIOnlyInputMode;
//	SetInputMode(UIOnlyInputMode);
	//HUD 초기화

	AQLCharacterPlayer* QLCharacter = Cast<AQLCharacterPlayer>(GetPawn());
	if (QLCharacter)
	{
		QLCharacter->OnChangeShootingMethod.BindUObject(Widget, &UQLUserWidget::VisibleShootingMethodUI);
	}
}

void AQLPlayerController::UpdateAmmoUI(UObject* Item,int32 UpdateItemCnt)
{
	UQLInventory* InventoryUI = Cast<UQLInventory>(HUDs[EHUDType::Inventory]);
	InventoryUI->HasCurrentItem(Item);
	
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

void AQLPlayerController::AddInventoryByDraggedItem(EItemType ItemIdx, int32 CurrentItemCnt)
{
	//Player전달
	AQLCharacterPlayer* QLCharacter = Cast<AQLCharacterPlayer>(GetPawn());

	if (QLCharacter)
	{
		QL_LOG(QLNetLog, Warning, TEXT("Add item %d"), CurrentItemCnt);

		QLCharacter->GetInventory()->AddInventoryByDraggedItem(ItemIdx, CurrentItemCnt);
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
		QL_LOG(QLNetLog, Warning, TEXT("Drop item %d"),CurrentItemCnt);
		QLCharacter->GetInventory()->AddGroundByDraggedItem(ItemIdx, CurrentItemCnt);
	}
}
