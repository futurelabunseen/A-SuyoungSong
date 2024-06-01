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
	
	virtual void NativeConstruct() override; //AddToViewport 함수가 외부에서 호출될 때 수행.

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	
	FTimerHandle ShowTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UOverlay> MapOverlay;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UImage> IMGPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class ACharacter> Player;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class ALocationVolume> LocationVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> StoageIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, TObjectPtr<class UUserWidget>> ItemStorage; //World에서 ItemStorage를 가져와서, ItemStorage의 id값을 가지고 map 생성
	
	UPROPERTY()
	TMap<int32, TWeakObjectPtr<class AQLLifestoneStorageBox>> StorageBoxes; //모든 박스를 가져와서 생명석이 들어있는 int32에 대해 10초동안 깜빡깜빡 거려라.
	
	uint8 bIsTwinkle : 1;
	void TwinkleWidget();
	void ResetShowTimer();
};
