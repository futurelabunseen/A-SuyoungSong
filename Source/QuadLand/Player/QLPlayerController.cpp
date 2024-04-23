// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/QLPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h"
#include "Player/QLPlayerState.h"
#include "UI/QLPlayerHUDWidget.h"
#include "UI/QLUserWidget.h"
#include "QuadLand.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "AttributeSet/QLAS_WeaponStat.h"

void AQLPlayerController::SetHiddenCrossHair()
{

	if (IsLocalController())
	{
		HUDs[EHUDType::CrossHair]->SetVisibility(ESlateVisibility::Hidden); //보이도록 함.
	}
}

void AQLPlayerController::BeginPlay()
{
	Super::BeginPlay();

}
void AQLPlayerController::SetVisibilityCrossHair()
{
	if (IsLocalController())
	{
		HUDs[EHUDType::CrossHair]->SetVisibility(ESlateVisibility::Visible);
	}
}

void AQLPlayerController::SetVisibilityInventory()
{
	if (IsLocalController())
	{
		HUDs[EHUDType::Inventory]->SetVisibility(ESlateVisibility::Visible);
	}
}
void AQLPlayerController::SetHiddenInventory()
{
	if (IsLocalController())
	{
		HUDs[EHUDType::Inventory]->SetVisibility(ESlateVisibility::Hidden);
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

	HUDs[EHUDType::Inventory]->SetVisibility(ESlateVisibility::Hidden);
	//HUD 초기화
}