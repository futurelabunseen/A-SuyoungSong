// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/QLHUD.h"

#include "UI/QLUserWidget.h"
#include "UI/QLLoadingPanel.h"
#include "UI/QLDeathTimerWidget.h"
#include "UI/QLMap.h"
#include "UI/QLBloodWidget.h"

#include "Character/QLCharacterPlayer.h"
#include "Player/QLPlayerState.h"
#include "Blueprint/UserWidget.h"

#include "Game/QLGameInstance.h"
#include "GameData/QLDataManager.h"
#include "QuadLand.h"

//AHUD Ŭ���� ��ü�� Ŭ���̾�Ʈ������ �����ȴ�.
void AQLHUD::BeginPlay()
{
	Super::BeginPlay();
}

/*HUD Ŭ������ Ŭ���̾�Ʈ �󿡼��� �����ϱ� ������ IsLocalController�� �� �ʿ䰡 ����, ������ ȣ���ϱ� �� �������� ȣ�� �� �� �ֱ� ������ nullptr������ Ȯ��*/
void AQLHUD::SetHiddenHUD(EHUDType UItype)
{
	if (HUDs.Find(UItype))
	{
		HUDs[UItype]->SetVisibility(ESlateVisibility::Collapsed);
	}
}
void AQLHUD::SetVisibilityHUD(EHUDType UItype)
{
	if (HUDs.Find(UItype))
	{
		HUDs[UItype]->SetVisibility(ESlateVisibility::Visible);
	}
}

void AQLHUD::ChangedAmmoCnt(float CurrentAmmo)
{   
	UQLUserWidget* Widget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);
	if (Widget)
	{
		Widget->ChangedAmmoCnt(CurrentAmmo);
	}
}

void AQLHUD::ChangedHPPercentage(float Hp, float MaxHp)
{        
	UQLUserWidget* Widget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (Widget)
	{
		Widget->ChangedHPPercentage(Hp, MaxHp);
	}
}

void AQLHUD::ChangedStaminaPercentage(float Stamina, float MaxStamina)
{
	UQLUserWidget* Widget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (Widget)
	{
		Widget->ChangedStaminaPercentage(Stamina, MaxStamina);
	}
}

void AQLHUD::ChangedRemainingAmmoCnt(float RemainigAmmo)
{
	UQLUserWidget* Widget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);
	if (Widget)
	{
		Widget->ChangedRemainingAmmo(RemainigAmmo);
	}
}

void AQLHUD::InitStoneTexture(int GemType)
{
	UQLUserWidget* UserWidget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);
	UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());

	if (UserWidget)
	{
		UserWidget->SettingStoneImg(DataManager->GemTexture(GemType));
	}
}

void AQLHUD::SettingNickname()
{
	if (LocalPlayerState)
	{
		UQLUserWidget* UserWidget = Cast< UQLUserWidget>(HUDs[EHUDType::HUD]);
		UserWidget->SettingNickname(LocalPlayerState->GetPlayerName());
	}
}

void AQLHUD::ResetUI()
{
	if (HUDs.Num() == 0) return;

	UQLUserWidget* UserWidget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);
	if (UserWidget)
	{
		UserWidget->UpdateEquipWeaponUI(false);
		UserWidget->UpdateEquipBombUI(false);
	}
}

void AQLHUD::CloseAllUI()
{
	for (const auto HUD : HUDs)
	{
		HUD.Value->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AQLHUD::InitWidget()
{
	//�������� ������ �ϸ��
	if (HUDs.Find(EHUDType::HUD))
	{
		UQLUserWidget* Widget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

		AQLCharacterPlayer* QLCharacter = Cast<AQLCharacterPlayer>(GetOwningPawn());
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

void AQLHUD::ReduceDeathSec(float Time)
{
	//if (IsLocalController())
	{
		UQLDeathTimerWidget* DeathTimerWidget = Cast<UQLDeathTimerWidget>(HUDs[EHUDType::DeathTimer]);
		DeathTimerWidget->UpdateTimer(Time);
	}

}

void AQLHUD::ShowStoneUI(float Time)
{
	UQLMap* Map = Cast<UQLMap>(HUDs[EHUDType::Map]);

	if (Map)
	{
		Map->ShowLifestoneBox(Time);
	}
}

void AQLHUD::SwitchWeaponStyle(ECharacterAttackType AttackType)
{
	UQLUserWidget* UserWidget = Cast< UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (UserWidget)
	{
		UserWidget->SwitchWeaponStyle(AttackType);
	}
}

void AQLHUD::SetUpdateLivePlayer(int16 InLivePlayer)
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

void AQLHUD::UpdateLoadingSec(float SecondsLeft)
{
	if (HUDs.Find(EHUDType::Loading) == false) return; //Client�� �������� HUD �ߴ� �ð��� ���� �ɸ��� ������, Update�ϴٰ� ��� nullptr �ߴ� �� ����.
	UQLLoadingPanel* UserWidget = Cast<UQLLoadingPanel>(HUDs[EHUDType::Loading]);
	if (UserWidget)
	{
		UserWidget->SetTxtRemainingTime(SecondsLeft);
	}
}

void AQLHUD::CreateHUD()
{
	if (HUDClass.Num() == 0) return;

	LocalPlayerState = PlayerOwner->GetPlayerState<AQLPlayerState>();

	if (!LocalPlayerState)
	{
		QL_LOG(QLNetLog, Warning, TEXT("PlayerState is not founded"));
		return;
	}

	for (const auto& HUD : HUDClass)
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), HUD.Value);
		Widget->AddToViewport(0);
		Widget->SetVisibility(ESlateVisibility::Visible);
		HUDs.Add(HUD.Key, Widget);
	}

	UQLUserWidget* Widget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (Widget)
	{
		Widget->ChangedAmmoCnt(LocalPlayerState->GetCurrentAmmoCnt());
		Widget->ChangedRemainingAmmo(LocalPlayerState->GetMaxAmmoCnt()); //�ӽð� ����
		Widget->ChangedHPPercentage(LocalPlayerState->GetHealth(), LocalPlayerState->GetMaxHealth());
		Widget->ChangedStaminaPercentage(LocalPlayerState->GetStamina(), LocalPlayerState->GetMaxStamina());
		SettingNickname();
	}

	SetHiddenHUD(EHUDType::Map);
	SetHiddenHUD(EHUDType::DeathTimer);
	SetHiddenHUD(EHUDType::Blood);
	SetHiddenHUD(EHUDType::KeyGuide);

	UQLGameInstance* GameInstance = Cast<UQLGameInstance>(GetWorld()->GetGameInstance());

	if (GameInstance)
	{
		InitStoneTexture(GameInstance->GetGemMatType());
	}

	//Delegate ����
	LocalPlayerState->UpdateAmmoDelegate.BindUObject(this, &AQLHUD::ChangedAmmoCnt);
	LocalPlayerState->UpdateHPDelegate.BindUObject(this, &AQLHUD::ChangedHPPercentage);
	LocalPlayerState->UpdateRemainingDelegate.BindUObject(this, &AQLHUD::ChangedRemainingAmmoCnt);
	LocalPlayerState->UpdateStaminaDelegate.BindUObject(this, &AQLHUD::ChangedStaminaPercentage);
}
void AQLHUD::UpdateEquipWeaponUI(bool InVisible)
{
	UQLUserWidget* UserWidget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (UserWidget)
	{
		UserWidget->UpdateEquipWeaponUI(InVisible);
	}
}

void AQLHUD::UpdateEquipBombUI(bool InVisible)
{
	UQLUserWidget* UserWidget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (UserWidget)
	{
		UserWidget->UpdateEquipBombUI(InVisible);
	}
}

void AQLHUD::ConcealLifeStone(bool InVisible)
{
	UQLUserWidget* UserWidget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (UserWidget)
	{
		UserWidget->ConcealLifeStone(InVisible);
	}
}

void AQLHUD::UpdateProgressTime(const FString &ProgressTimeText)
{
	if (HUDs.Find(EHUDType::HUD) == 0) return;

	UQLUserWidget* UserWidget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);
	UserWidget->UpdateProgressTime(ProgressTimeText);
}

void AQLHUD::BlinkBloodWidget()
{
	SetVisibilityHUD(EHUDType::Blood);
	UQLBloodWidget* Widget = Cast<UQLBloodWidget>(HUDs[EHUDType::Blood]);

	if (Widget)
	{
		Widget->BlinkWidget();
	}
	FTimerHandle CancelTimer;

	GetWorld()->GetTimerManager().SetTimer(CancelTimer, this,&AQLHUD::CancelBloodWidget, 3.0f, false);
}

void AQLHUD::CancelBloodWidget()
{
	UQLBloodWidget* Widget = Cast<UQLBloodWidget>(HUDs[EHUDType::Blood]);

	if (Widget)
	{
		Widget->CancelWidget();
	}
	SetHiddenHUD(EHUDType::Blood);
}

void AQLHUD::BlinkBag()
{
	UQLUserWidget* UserWidget = Cast< UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (UserWidget)
	{
		UserWidget->BlinkBag();
	}
}