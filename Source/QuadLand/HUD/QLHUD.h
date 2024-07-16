// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/QLUIType.h"
#include "QLHUD.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLHUD : public AHUD
{
	GENERATED_BODY()
public:

	void CreateHUD();
	virtual void BeginPlay() override;

	/*HUD 를 여는 용*/
	UFUNCTION(BlueprintCallable)
	void SetVisibilityHUD(EHUDType UItype);
	/*HUD 를 닫는 용*/
	UFUNCTION(BlueprintCallable)
	void SetHiddenHUD(EHUDType UItype);
	
	//현재 HUD의 개수
	int HUDNum() {
		return HUDs.Num();
	}

	/*PlayerController에서 로딩된 초를 업데이트하는 용도*/
	void UpdateLoadingSec(float Time);
protected:
	/*보석 관련 HUD 변경*/
	void InitStoneTexture(int GemType);
	/*닉네임 관련 HUD 변경*/
	void SettingNickname();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TMap<EHUDType, TObjectPtr<class UUserWidget>> HUDs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TMap<EHUDType, TSubclassOf<class UUserWidget>> HUDClass;

protected:
	/*2번 이상 사용하는 PlayerState 를 미리 저장해 놓자.*/
	UPROPERTY()
	TObjectPtr<class AQLPlayerState> LocalPlayerState;

};
