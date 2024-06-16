// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLMap.h"
#include "Components/Overlay.h"
#include "EngineUtils.h"
#include "Gimmick/QLLifestoneStorageBox.h"
#include "Gimmick/QLMapBoundary.h"
#include "Components/Image.h"
#include "Widgets/SWidget.h"
#include "LocationVolume.h"
#include "Components/BrushComponent.h"
#include "GameFramework/Character.h"

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


		ALocationVolume* Location = Cast<ALocationVolume>(Entry);
		if (Location)
		{
			LocationVolume = Location;
		}

	}
	//Player = Cast<ACharacter>(GetOwningPlayerPawn());
}


void UQLMap::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (LocationVolume == nullptr)
	{
		UWorld* CurrentWorld = GetWorld();
		for (const auto& Entry : FActorRange(CurrentWorld))
		{
			ALocationVolume* Location = Cast<ALocationVolume>(Entry);
			if (Location)
			{
				LocationVolume = Location;
			}
		}
	}
	if (LocationVolume && Player)
	{
		FVector LocationPivot=LocationVolume->GetActorLocation();
		FVector2D VolumeScale(15500.0f, 9000.0f);

		LocationPivot.X = LocationPivot.X - (VolumeScale.X / 2.f); //202
		LocationPivot.Y = LocationPivot.Y - (VolumeScale.Y / 2.f);
		
		FVector PlayerPivot = Player->GetActorLocation();

		float MagnitudeX= (PlayerPivot.X - LocationPivot.X) * (1000.0f/VolumeScale.X);
		float MagnitudeY = (PlayerPivot.Y - LocationPivot.Y) * (600.0f/VolumeScale.Y);

		IMGPlayer->SetRenderTranslation(FVector2D(MagnitudeX, MagnitudeY));
	}
}

void UQLMap::ResetPlayer()
{
	Player = Cast<ACharacter>(GetOwningPlayerPawn());

}
