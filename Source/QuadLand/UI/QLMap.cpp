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
			FVector ActorLocation = StorageBox->GetActorLocation();
			Widget->SetRenderTranslation(StorageBox->GetStorageWidgetLocation());
			Widget->SetVisibility(ESlateVisibility::Hidden);
			MapOverlay->AddChildToOverlay(Widget);
			ItemStorage.Add(Index, Widget);
			Index++;
		}
	}
}


// �������� ����� �� StorageBoxes �� Ȯ���ϰ� ���ִٸ� �� Index�� ���������ϵ��� �ϱ�
// ��������Ʈ�� ���� or PlayerController ���� 