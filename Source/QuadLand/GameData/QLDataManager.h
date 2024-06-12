// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/WeaponType.h"
#include "GameData/QLItemType.h"
#include "Subsystems/WorldSubsystem.h"
#include "QLDataManager.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLDataManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UQLDataManager();

	const class UQLWeaponStat* GetWeaponStat(ECharacterAttackType AttackType);
	class UQLItemData* GetItem(EItemType ItemId);
	TSubclassOf<class AQLItemBox> GetItemBoxClass(EItemType ItemId);


	class  USkeletalMesh* GetSkeletalMesh(int Idx);
	const class UQLGemData* GetGemData(int Idx);
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY()
	TObjectPtr<class UQLSelectionData> InitDataManager;

	UPROPERTY()
	TObjectPtr<class UQLItemDataset> ItemDataManager; //���߿� TMap�� ����ؼ� ����Ŭ���� - ����Ŭ������ �ΰų�, �������̽��� ��� ����
	
	UPROPERTY()
	TObjectPtr<class UQLWeaponDataset> WeaponStatManager; //���߿� TMap�� ����ؼ� ����Ŭ���� - ����Ŭ������ �ΰų�, �������̽��� ��� ����

};

