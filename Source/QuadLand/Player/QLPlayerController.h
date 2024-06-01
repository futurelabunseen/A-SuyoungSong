// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/QLUIType.h"
#include "GameData/WeaponType.h"
#include "GameData/QLItemType.h"
#include "AbilitySystemInterface.h"
#include "QLPlayerController.generated.h"

/**
 * 
 */

DECLARE_DELEGATE(FOnDeathCheckDelegate)
UCLASS()
class QUADLAND_API AQLPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void SetVisibilityHUD(EHUDType UItype);

	UFUNCTION(BlueprintCallable)
	void SetHiddenHUD(EHUDType UItype);

	const class UUserWidget* GetCrossHairUIWidget() const { return HUDs[EHUDType::CrossHair]; }
	class UUserWidget* GetPlayerUIWidget() const { return HUDs[EHUDType::HUD]; }

	void CreateHUD();
	void UpdateAmmoUI(UObject* Item,int32 UpdateItemCnt);
	/* 인벤토리 */
	void UpdateNearbyItemEntry(UObject* Item); //아이템이 생김 - Stat을 넘기자
	void UpdateItemEntry(UObject* Item, int32 CurrentItemCnt);


	void UpdateEquipWeaponUI();
	void UpdateEquipBombUI();

	void AddInventoryByDraggedItem(EItemType ItemIdx,int32 CurrentItemCnt);
	void RemoveItemEntry(EItemType ItemIdx); //선택된 arr값 - id를 넘겨서 제거하자
	void AddGroundByDraggedItem(EItemType ItemIdx, int32 CurrentItemCnt);
	void ConcealLifeStone();

	UFUNCTION(BlueprintCallable)
	void CloseHUD(EHUDType UItype);

	UFUNCTION(Client,Unreliable)
	void ClientRPCShowLifestoneWidget(float Timer);
	
	void ActivateDeathTimer(float Time);

	UFUNCTION(Client, Reliable)
	void ClientRPCGameStart();

	void BlinkBloodWidget();
	void CancelBloodWidget();

	void BlinkBag();

	UFUNCTION(BlueprintCallable)
	void CloseInventroy();

	void SwitchWeaponStyle(ECharacterAttackType AttackType);

	FOnDeathCheckDelegate OnDeathCheckDelegate;
protected:

	uint8 bIsBlinkWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TMap<EHUDType,TSubclassOf<class UUserWidget>> HUDClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TMap<EHUDType, TObjectPtr<class UUserWidget>> HUDs;

	FTimerHandle DeathTimerHandle;
	
	int CurrentDeathSec;
	void ReduceDeathSec();
	void StopDeathSec();
};
