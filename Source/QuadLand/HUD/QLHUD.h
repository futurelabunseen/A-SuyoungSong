// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/QLUIType.h"
#include "GameData/WeaponType.h"
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
	
	class UUserWidget* GetPlayerUIWidget() const
	{
		if (HUDs.Find(EHUDType::HUD) == 0) return nullptr;
		return HUDs[EHUDType::HUD];
	}

	void ResetUI();
	void CloseAllUI();
	void InitWidget();

	void ReduceDeathSec(float Time);
	void ShowStoneUI(float Time);
	void SwitchWeaponStyle(ECharacterAttackType AttackType);
	void SetUpdateLivePlayer(int16 InLivePlayer);
	void UpdateEquipWeaponUI(bool InVisible);
	void UpdateEquipBombUI(bool InVisible);
	void ConcealLifeStone(bool InVisible);
	//경쟁 시간 체크
	void UpdateProgressTime(const FString &ProgressTime);
	/*PlayerController에서 로딩된 초를 업데이트하는 용도*/
	void UpdateLoadingSec(float Time);

	void BlinkBloodWidget();
	void CancelBloodWidget();

	void BlinkBag();

protected:

	/*Attribute Set - Widget 연결을 위한 함수*/
	void ChangedAmmoCnt(float CurrentAmmo);
	void ChangedHPPercentage(float Hp,float MaxHp);
	void ChangedStaminaPercentage(float Stamina, float MaxStamina);
	void ChangedRemainingAmmoCnt(float RemainigAmmo);

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
