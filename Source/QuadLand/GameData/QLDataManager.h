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
	TObjectPtr<class UQLItemDataset> ItemDataManager; //나중엔 TMap을 사용해서 상위클래스 - 하위클래스로 두거나, 인터페이스로 묶어서 관리
	
	UPROPERTY()
	TObjectPtr<class UQLWeaponDataset> WeaponStatManager; //나중엔 TMap을 사용해서 상위클래스 - 하위클래스로 두거나, 인터페이스로 묶어서 관리

};

