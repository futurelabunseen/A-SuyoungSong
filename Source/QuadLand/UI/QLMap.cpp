// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLMap.h"
#include "Components/Overlay.h"
#include "EngineUtils.h"
#include "Gimmick/QLLifestoneStorageBox.h"
#include "Gimmick/QLMapBoundary.h"
#include "Components/Image.h"
#include "Widgets/SWidget.h"
#include "GameFramework/Character.h"

#include "LocationVolume.h"
#include "Components/BrushComponent.h"
#include "Engine/Brush.h"
#include "Engine/BrushBuilder.h"

UQLMap::UQLMap(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UIWidth = 1000.f;
	UIHeight = 600.f;
	SetKeyboardFocus();
}

void UQLMap::ShowLifestoneBox(float Timer)
{
	//10�ʰ� ���������� �����Ѵ�. -> �̹� �����߿� ������ ���� ������ �߰��ؾ���.
	//Ÿ�̸� 2���� ����ؼ�, �ϳ��� ��ü 10�ʸ� �ϳ��� ���������� ����ϵ��� ������.
	FTimerHandle TwinkleShowControllTimer;

	if (TwinkleShowControllTimer.IsValid() == false)
	{//��ȿ���� ���� �� ����
		GetWorld()->GetTimerManager().SetTimer(ShowTimerHandle, this, &UQLMap::TwinkleWidget, 0.5f, true, 1.f);
	}
	//UI�̱⶧���� �������� ������ �ʿ����.
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
			MapOverlay->AddChildToOverlay(Widget);
			ItemStorage.Add(Index, Widget);
			Index++;
			Widget->SetRenderTranslation(StorageBox->GetStorageWidgetLocation());
			Widget->SetVisibility(ESlateVisibility::Hidden);
			
		}

		CalMapRatio(Entry);
	}
}


void UQLMap::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (LocationVolume == nullptr)
	{
		UWorld* CurrentWorld = GetWorld();
		for (const auto& Entry : FActorRange(CurrentWorld))
		{
			CalMapRatio(Entry);
		}
	}

	if (Player)
	{
		FVector PlayerPivot = Player->GetActorLocation();

		float MagnitudeX= (PlayerPivot.X - LocationPivot.X) * RatioWidth;
		float MagnitudeY = (PlayerPivot.Y - LocationPivot.Y) * RatioHeight;

		IMGPlayer->SetRenderTranslation(FVector2D(MagnitudeX, MagnitudeY));
	}
}

void UQLMap::ResetPlayer()
{
	Player = Cast<ACharacter>(GetOwningPlayerPawn());
}

void UQLMap::CalMapRatio(AActor *Entry)
{
	ALocationVolume* Location = Cast<ALocationVolume>(Entry);
	if (Location)
	{
		LocationVolume = Location;
		LocationPivot = LocationVolume->GetActorLocation();

		//Boxũ��� �߾������� ���� 2��
		float Width = Location->Brush->Bounds.BoxExtent.X; 
		float Height = Location->Brush->Bounds.BoxExtent.Y;

		//����, ���� ��ǥ�� ���ϱ� ���ؼ�, �ڽ� ���ݰ��� ����
		LocationPivot.X = LocationPivot.X - Width; 		
		LocationPivot.Y = LocationPivot.Y - Height;

		RatioWidth = UIWidth/(Width*2);
		RatioHeight = UIHeight/(Height*2);
	}
}
