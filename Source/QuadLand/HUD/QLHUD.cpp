// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/QLHUD.h"
#include "QuadLand.h"
#include "Player/QLPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "UI/QLUserWidget.h"
#include "UI/QLLoadingPanel.h"
#include "Game/QLGameInstance.h"
#include "GameData/QLDataManager.h"

//AHUD 클래스 자체는 클라이언트에서만 생성된다.
void AQLHUD::BeginPlay()
{
	Super::BeginPlay();
}

/*HUD 클래스는 클라이언트 상에서만 존재하기 때문에 IsLocalController를 할 필요가 없음, 하지만 호출하기 전 서버에서 호출 할 수 있기 때문에 nullptr인지를 확인*/
void AQLHUD::SetHiddenHUD(EHUDType UItype)
{
	if (HUDs.Find(UItype))
	{
		HUDs[UItype]->SetVisibility(ESlateVisibility::Hidden);
	}
}
void AQLHUD::SetVisibilityHUD(EHUDType UItype)
{
	if (HUDs.Find(UItype))
	{
		HUDs[UItype]->SetVisibility(ESlateVisibility::Visible);
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

void AQLHUD::UpdateLoadingSec(float SecondsLeft)
{
	if (HUDs.Find(EHUDType::Loading) == false) return; //Client가 서버보다 HUD 뜨는 시간이 오래 걸리기 때문에, Update하다가 없어서 nullptr 뜨는 걸 막음.
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

	UQLUserWidget* Widget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

	if (Widget)
	{
		Widget->ChangedAmmoCnt(LocalPlayerState->GetCurrentAmmoCnt());
		Widget->ChangedRemainingAmmo(LocalPlayerState->GetMaxAmmoCnt()); //임시값 삽입
		Widget->ChangedHPPercentage(LocalPlayerState->GetHealth(), LocalPlayerState->GetMaxHealth());
		Widget->ChangedStaminaPercentage(LocalPlayerState->GetStamina(), LocalPlayerState->GetMaxStamina());
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

