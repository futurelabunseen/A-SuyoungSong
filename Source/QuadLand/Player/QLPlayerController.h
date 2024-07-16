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
DECLARE_DELEGATE(FOnCoolTimeStamina)
DECLARE_DELEGATE(FOnResetStamina)
DECLARE_DELEGATE(FOnDeathCheckDelegate)
UCLASS()
class QUADLAND_API AQLPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AQLPlayerController();

	virtual void BeginPlay() override;
	void Inventory();
	UFUNCTION(BlueprintCallable)
	void SetVisibilityHUD(EHUDType UItype);
	
	UFUNCTION(BlueprintCallable)
	void SetHiddenHUD(EHUDType UItype);
	int HUDNum() {
		return HUDs.Num();
	}

	const class UUserWidget* GetCrossHairUIWidget() const 
	{
		if (HUDs.Find(EHUDType::CrossHair) == 0) return nullptr;
		return HUDs[EHUDType::CrossHair]; 
	}
	class UUserWidget* GetPlayerUIWidget() const
	{ 
		if (HUDs.Find(EHUDType::HUD) == 0) return nullptr;
		return HUDs[EHUDType::HUD];
	}

	UFUNCTION(Client,Reliable)
	void ClientRPCOutLobby();
	void CreateHUD();

	/* �κ��丮 */
	void UpdateNearbyItemEntry(UObject* Item); //�������� ���� - Stat�� �ѱ���
	void UpdateItemEntry(UObject* Item, int32 CurrentItemCnt);

	void UpdateEquipWeaponUI(bool InVisible);
	void UpdateEquipBombUI(bool InVisible);

	void AddInventoryByDraggedItem(EItemType ItemIdx);
	void RemoveItemEntry(EItemType ItemIdx); //���õ� arr�� - id�� �Ѱܼ� ��������
	void AddGroundByDraggedItem(EItemType ItemIdx);
	void ConcealLifeStone(bool InVisible);

	virtual void OnPossess(APawn* aPawn) override;
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

//	void InitStoneTexture(int GemType);
	void Win();
	UFUNCTION(Client,Reliable)
	void ClientRPCLoose();

	UFUNCTION(BlueprintCallable)
	void CloseInventroy();

	void SwitchWeaponStyle(ECharacterAttackType AttackType);
	void SetUpdateLivePlayer(int16 InLivePlayer);
	void SettingDeathTime();

	FOnDeathCheckDelegate OnDeathCheckDelegate;
	void RemoveAllNearbyItemEntries(); //���� ����
	void RemoveNearbyItemEntry(UObject* Item); //���� ����
	uint8 bReadyGame : 1;

	UFUNCTION(Client,Reliable)
	void ClientRPCUpdateLivePlayer(int16 InLivePlayer); //GameMode�κ��� ClientRPC�� ���޹���.

//	void SettingNickname();

	UFUNCTION(Client, Reliable)
	void ClientRPCCreateWidget();

	void InitWidget();
	
	UFUNCTION(Server, Reliable)
	void ServerRPCInitPawn(int Type);
	FString ChangeTimeText();

	void ResetUI();
	void CloseAllUI();
	void PlayCanSound();


	FOnCoolTimeStamina CoolTimeStamina;
	FOnResetStamina OnResetStamina;

	uint8 bIsDanger : 1;
	float ElapsedTime = 0.0f;
	float CheckInterval = 10.0f; // 10�� ����
protected:

	FString DeathTime;

	uint8 bIsBlinkWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TMap<EHUDType,TSubclassOf<class UUserWidget>> HUDClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TMap<EHUDType, TObjectPtr<class UUserWidget>> HUDs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TMap<EHUDType, TSubclassOf<class UUserWidget>> LateHUDClass;


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
	float MatchTime = 10.f;

	uint32 CountDownInt = 0;

	float StartTime = 0.f;
	void SetHUDTime();
	uint8 bStartGame : 1;




};
