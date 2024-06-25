// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "QLGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UQLGameInstance();

	void SetGenderType(int InGenderType) { GenderType = InGenderType; }
	void SetGemType(int MatType) { GemMatType = MatType; }

	int GetGenderType() { return GenderType; }
	int GetGemMatType() { return GemMatType; }

protected:

	virtual void Init();

	//�κ��丮 �ý����� ���� ������ �Ŵ���
	UPROPERTY()
	TObjectPtr<class UQLDataManager> DataManager;
	//���� Ÿ�԰� ���� Ÿ���� ���� �Ѿ �� �����ϱ� ���ؼ� ���� �ν��Ͻ� ���
	int GenderType; 
	int GemMatType;
};
