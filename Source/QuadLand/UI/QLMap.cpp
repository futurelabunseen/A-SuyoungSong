// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLMap.h"
#include "Components/Overlay.h"
#include "EngineUtils.h"
#include "Gimmick/QLLifestoneStorageBox.h"

UQLMap::UQLMap(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetKeyboardFocus();
}

void UQLMap::ShowLifestoneBox(float Timer)
{
	//10초간 깜빡깜빡을 진행한다. -> 이미 진행중에 있으면 막는 로직도 추가해야함.
	//타이머 2개를 사용해서, 하나는 전체 10초를 하나는 깜빡깜빡을 담당하도록 구현함.
	FTimerHandle TwinkleShowControllTimer;

	if (TwinkleShowControllTimer.IsValid() == false)
	{//유효하지 않을 때 실행
		GetWorld()->GetTimerManager().SetTimer(ShowTimerHandle, this, &UQLMap::TwinkleWidget, 0.5f, true, 1.f);
	}
	//UI이기때문에 서버에서 관리할 필요없음.
	GetWorld()->GetTimerManager().SetTimer(TwinkleShowControllTimer, this, &UQLMap::ResetShowTimer, Timer, false,-1.0f);
}

void UQLMap::ResetShowTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(ShowTimerHandle);
	ShowTimerHandle.Invalidate();
	for (const auto& StorageBox : StorageBoxes)
	{
		if (StorageBox.Value->GetAlreadyHidden())
		{
			ItemStorage[StorageBox.Key]->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UQLMap::TwinkleWidget()
{
	bIsTwinkle = !bIsTwinkle;

	if (bIsTwinkle)
	{
		for (const auto& StorageBox : StorageBoxes)
		{

			if (StorageBox.Value->GetAlreadyHidden())
			{
				ItemStorage[StorageBox.Key]->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
	else
	{
		for (const auto& StorageBox : StorageBoxes)
		{
			if (StorageBox.Value->GetAlreadyHidden())
			{
				ItemStorage[StorageBox.Key]->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void UQLMap::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* CurrentWorld = GetWorld();

	int32 Index = 1;
	for (const auto& Entry : FActorRange(CurrentWorld))
	{
		AQLLifestoneStorageBox* StorageBox = Cast< AQLLifestoneStorageBox>(Entry);

		if (StorageBox)
		{

			StorageBoxes.Add(Index,StorageBox);

			UUserWidget* Widget = CreateWidget<UUserWidget>(this, StoageIcon);
			FVector ActorLocation = StorageBox->GetActorLocation();
			Widget->SetRenderTranslation(StorageBox->GetStorageWidgetLocation());
			Widget->SetVisibility(ESlateVisibility::Hidden);
			MapOverlay->AddChildToOverlay(Widget);
			ItemStorage.Add(Index, Widget);
			Index++;
		}
	}
}


// 아이템을 사용할 때 StorageBoxes 를 확인하고 차있다면 그 Index는 깜빡깜빡하도록 하기
// 델리게이트로 연결 or PlayerController 지시 