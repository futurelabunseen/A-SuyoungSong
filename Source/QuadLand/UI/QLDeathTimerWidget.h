// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QLDeathTimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLDeathTimerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void UpdateTimer(int Time); //Ÿ�̸� 10�ʰ� ������Ʈ

	//void SetTxt(); //static ? �ϴ��غ��� �����غ�
	//void ClearTimer();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timer, meta = (BindWidget))
	TObjectPtr<class UTextBlock> TxtTimer;
};
