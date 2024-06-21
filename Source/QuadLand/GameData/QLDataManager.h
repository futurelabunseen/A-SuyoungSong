// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/WeaponType.h"
#include "GameData/QLItemType.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "QLDataManager.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLDataManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UQLDataManager();

	const class UQLWeaponStat* GetWeaponStat(ECharacterAttackType AttackType);
	class UQLItemData* GetItem(EItemType ItemId);
	TSubclassOf<class AQLItemBox> GetItemBoxClass(EItemType ItemId);

	TSubclassOf<class AQLCharacterPlayer> GetSkeletalMesh(int Idx);
	
	class UTexture2D* GemTexture(int Type);
	
	TSubclassOf<class AQLPlayerLifeStone> GetLifeStoneClass(int Type);

	TObjectPtr<class USoundCue>  PlayMoanSound(int Idx);
	TObjectPtr<class USoundCue>  PlayJumpSound(int Idx);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY()
	TObjectPtr<class UQLSelectionData> InitDataManager;

	UPROPERTY()
	TObjectPtr<class UQLItemDataset> ItemDataManager; //���߿� TMap�� ����ؼ� ����Ŭ���� - ����Ŭ������ �ΰų�, �������̽��� ��� ����
	
	UPROPERTY()
	TObjectPtr<class UQLWeaponDataset> WeaponStatManager; //���߿� TMap�� ����ؼ� ����Ŭ���� - ����Ŭ������ �ΰų�, �������̽��� ��� ����

};

