// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLMap.h"
#include "Components/Overlay.h"
#include "EngineUtils.h"
#include "Gimmick/QLLifestoneStorageBox.h"
#include "Gimmick/QLMapBoundary.h"
#include "Components/Image.h"
#include "Widgets/SWidget.h"

UQLMap::UQLMap(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
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
	
	for (int i = 1; i <= 4; i++)
	{
		FString WidgetName = FString::Printf(TEXT("RatioCalculatorUI_%d"), i);
		UImage* Widget = Cast<UImage>(GetWidgetFromName(FName(WidgetName)));

		if (Widget)
		{
			UE_LOG(LogTemp, Log, TEXT("Widget Name %s"), *WidgetName);
			RatioCalculatorUI.Add(WidgetName, Widget);
		}
	}

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

		AQLMapBoundary* Boundary = Cast<AQLMapBoundary>(Entry);
		if (Boundary)
		{
			if (RatioCalculatorUI.Find(Boundary->RatioCalculatorUI))
			{
				UImage* WidgetBoundary = RatioCalculatorUI[Boundary->RatioCalculatorUI];

				FVector2D WidgetLocation = WidgetBoundary->GetRenderTransform().Translation;
				FVector BoundaryLocation = Boundary->GetLocation();

				float WorldX = BoundaryLocation.Y;
				float WorldY = BoundaryLocation.X;

				float X = 1000.0f - WidgetLocation.X;
				float Y = WidgetLocation.Y;
				//�Ѵ� ������ ���� üũ 
				UE_LOG(LogTemp, Log, TEXT("%s Map (%s) Widget (X=%f Y=%f)"),*Boundary->GetName(), *Boundary->GetLocation().ToString(), X,Y);

				UE_LOG(LogTemp, Log, TEXT("Cal %f %f"), WorldX/X, WorldY/Y);

			}
			
		}
	}


}


// �������� ����� �� StorageBoxes �� Ȯ���ϰ� ���ִٸ� �� Index�� ���������ϵ��� �ϱ�
// ��������Ʈ�� ���� or PlayerController ���� 