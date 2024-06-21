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

	void CreateHUD();

	/* 인벤토리 */
	void UpdateNearbyItemEntry(UObject* Item); //아이템이 생김 - Stat을 넘기자
	void UpdateItemEntry(UObject* Item, int32 CurrentItemCnt);

	void UpdateEquipWeaponUI(bool InVisible);
	void UpdateEquipBombUI(bool InVisible);

	void AddInventoryByDraggedItem(EItemType ItemIdx,int32 CurrentItemCnt);
	void RemoveItemEntry(EItemType ItemIdx); //선택된 arr값 - id를 넘겨서 제거하자
	void AddGroundByDraggedItem(EItemType ItemIdx, int32 CurrentItemCnt);
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

	void InitStoneTexture(int GemType);
	void Win();
	UFUNCTION(Client,Reliable)
	void ClientRPCLoose();

	UFUNCTION(BlueprintCallable)
	void CloseInventroy();

	void SwitchWeaponStyle(ECharacterAttackType AttackType);
	void SetUpdateLivePlayer(int16 InLivePlayer);
	void SettingDeathTime();

	FOnDeathCheckDelegate OnDeathCheckDelegate;

	uint8 bReadyGame : 1;

	UFUNCTION(Client,Reliable)
	void ClientRPCUpdateLivePlayer(int16 InLivePlayer); //GameMode로부터 ClientRPC로 전달받음.

	void SettingNickname();

	UFUNCTION(Client, Reliable)
	void ClientRPCCreateWidget();
	
	UFUNCTION(Server, Reliable)
	void ServerRPCInitPawn(int Type);


	FString ChangeTimeText();

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

	//Client가 서버에게 요청한 시간을 전달.
	UFUNCTION(Server,Reliable)
	void ServerRPCRequestServerTime(float TimeOfClientRequest); //Client 요청한 시간

	//클라이언트에게 전달받은 시간, 서버가 요청받은 시간을 전달 해서 계산
	UFUNCTION(Client,Reliable)
	void ClientRPCReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f;
	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.0f;

	float TimeSyncRunningTime = 0.f;

	void ServerTimeCheck(float DeltaTime);

	//경쟁 시간 체크
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
