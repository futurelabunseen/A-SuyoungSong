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
	AQLPlayerController();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetVisibilityHUD(EHUDType UItype);

	int HUDNum() {
		return HUDs.Num();
	}
	UFUNCTION(BlueprintCallable)
	void SetHiddenHUD(EHUDType UItype);

	const class UUserWidget* GetCrossHairUIWidget() const { return HUDs[EHUDType::CrossHair]; }
	class UUserWidget* GetPlayerUIWidget() const { return HUDs[EHUDType::HUD]; }

	void CreateHUD();
	/* �κ��丮 */
	void UpdateNearbyItemEntry(UObject* Item); //�������� ���� - Stat�� �ѱ���
	void UpdateItemEntry(UObject* Item, int32 CurrentItemCnt);

	void UpdateEquipWeaponUI();
	void UpdateEquipBombUI();

	void AddInventoryByDraggedItem(EItemType ItemIdx,int32 CurrentItemCnt);
	void RemoveItemEntry(EItemType ItemIdx); //���õ� arr�� - id�� �Ѱܼ� ��������
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


	void Win();
	void Loose();

	UFUNCTION(BlueprintCallable)
	void CloseInventroy();

	void SwitchWeaponStyle(ECharacterAttackType AttackType);

	FOnDeathCheckDelegate OnDeathCheckDelegate;

	uint8 bReadyGame : 1;

	UFUNCTION(Client,Reliable)
	void ClientRPCUpdateLivePlayer(int16 InLivePlayer); //GameMode�κ��� ClientRPC�� ���޹���.

	void SettingNickname();

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

protected:
	//Client - Server Time Sync

	//Client�� �������� ��û�� �ð��� ����.
	UFUNCTION(Server,Reliable)
	void ServerRPCRequestServerTime(float TimeOfClientRequest); //Client ��û�� �ð�

	//Ŭ���̾�Ʈ���� ���޹��� �ð�, ������ ��û���� �ð��� ���� �ؼ� ���
	UFUNCTION(Client,Reliable)
	void ClientRPCReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f;
	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.0f;

	float TimeSyncRunningTime = 0.f;

	void ServerTimeCheck(float DeltaTime);

	//���� �ð� üũ
	void UpdateProgressTime();
public:

	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;

	virtual void Tick(float DeltaTime) override;
private:
	float MatchTime = 2.f;

	uint32 CountDownInt = 0;

	float StartTime = 0.f;
	void SetHUDTime();
	uint8 bStartGame : 1;



};
