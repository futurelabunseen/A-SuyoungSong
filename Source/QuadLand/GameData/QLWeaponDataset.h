// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/WeaponType.h"
#include "Engine/DataAsset.h"
#include "QLWeaponDataset.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLWeaponDataset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const class UQLWeaponStat* GetWeaponStat(ECharacterAttackType AttackType);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ECharacterAttackType, TObjectPtr<class UQLWeaponStat>> WeaponStatManager; //���߿� TMap�� ����ؼ� ����Ŭ���� - ����Ŭ������ �ΰų�, �������̽��� ��� ����
};
