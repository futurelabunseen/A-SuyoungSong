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

	/*HUD �� ���� ��*/
	UFUNCTION(BlueprintCallable)
	void SetVisibilityHUD(EHUDType UItype);
	/*HUD �� �ݴ� ��*/
	UFUNCTION(BlueprintCallable)
	void SetHiddenHUD(EHUDType UItype);
	
	//���� HUD�� ����
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
	//���� �ð� üũ
	void UpdateProgressTime(const FString &ProgressTime);
	/*PlayerController���� �ε��� �ʸ� ������Ʈ�ϴ� �뵵*/
	void UpdateLoadingSec(float Time);

	void BlinkBloodWidget();
	void CancelBloodWidget();

	void BlinkBag();

protected:

	/*Attribute Set - Widget ������ ���� �Լ�*/
	void ChangedAmmoCnt(float CurrentAmmo);
	void ChangedHPPercentage(float Hp,float MaxHp);
	void ChangedStaminaPercentage(float Stamina, float MaxStamina);
	void ChangedRemainingAmmoCnt(float RemainigAmmo);

protected:
	/*���� ���� HUD ����*/
	void InitStoneTexture(int GemType);
	/*�г��� ���� HUD ����*/
	void SettingNickname();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TMap<EHUDType, TObjectPtr<class UUserWidget>> HUDs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TMap<EHUDType, TSubclassOf<class UUserWidget>> HUDClass;

protected:
	/*2�� �̻� ����ϴ� PlayerState �� �̸� ������ ����.*/
	UPROPERTY()
	TObjectPtr<class AQLPlayerState> LocalPlayerState;

};
