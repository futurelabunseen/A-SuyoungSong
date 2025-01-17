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

	UFUNCTION(BlueprintCallable)
	void SetVisibilityHUD(EHUDType UItype);
	
	UFUNCTION(BlueprintCallable)
	void SetHiddenHUD(EHUDType UItype);
	int HUDNum() {
		return HUDs.Num();
	}
	virtual void SetPawn(APawn* InPawn) override;

	UFUNCTION(Client,Reliable)
	void ClientRPCOutLobby();
	void CreateHUD();

	/* 인벤토리 */
	void UpdateNearbyItemEntry(UObject* Item); //아이템이 생김 - Stat을 넘기자
	void UpdateItemEntry(UObject* Item, int32 CurrentItemCnt);


	void AddInventoryByDraggedItem(EItemType ItemIdx);
	void RemoveItemEntry(EItemType ItemIdx); //선택된 arr값 - id를 넘겨서 제거하자
	void AddGroundByDraggedItem(EItemType ItemIdx);
	void UpdateProgressTime();

	UFUNCTION(BlueprintCallable)
	void CloseHUD(EHUDType UItype);

	UFUNCTION(Client,Unreliable)
	void ClientRPCShowLifestoneWidget(float Timer);

	UFUNCTION(Client, Reliable)
	void ClientRPCGameStart();

	void ActivateDeathTimer(float Time);

//	void InitStoneTexture(int GemType);
	void Win();
	UFUNCTION(Client,Reliable)
	void ClientRPCLoose();

	UFUNCTION(BlueprintCallable)
	void CloseInventroy();

	void SetUpdateLivePlayer(int16 InLivePlayer);
	void SettingDeathTime();

	FOnDeathCheckDelegate OnDeathCheckDelegate;
	void RemoveAllNearbyItemEntries(); //전부 제거
	void RemoveNearbyItemEntry(UObject* Item); //전부 제거
	uint8 bReadyGame : 1;

	UFUNCTION(Client,Reliable)
	void ClientRPCUpdateLivePlayer(int16 InLivePlayer); //GameMode로부터 ClientRPC로 전달받음.

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
	float CheckInterval = 10.0f; // 10초 간격
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

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Player, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AQLCharacterPlayer> LocalPawn;

};
