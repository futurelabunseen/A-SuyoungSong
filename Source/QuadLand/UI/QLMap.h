// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QLMap.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLMap : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UQLMap(const FObjectInitializer& ObjectInitializer);

	void ShowLifestoneBox(float Timer);
	
	virtual void NativeConstruct(); //AddToViewport �Լ��� �ܺο��� ȣ��� �� ����.
protected:
	
	FTimerHandle ShowTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UOverlay> MapOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> StoageIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, TObjectPtr<class UUserWidget>> ItemStorage; //World���� ItemStorage�� �����ͼ�, ItemStorage�� id���� ������ map ����
	
	UPROPERTY()
	TMap<int32, TWeakObjectPtr<class AQLLifestoneStorageBox>> StorageBoxes; //��� �ڽ��� �����ͼ� ������ ����ִ� int32�� ���� 10�ʵ��� �������� �ŷ���.
	
	uint8 bIsTwinkle : 1;
	void TwinkleWidget();
	void ResetShowTimer();
};
