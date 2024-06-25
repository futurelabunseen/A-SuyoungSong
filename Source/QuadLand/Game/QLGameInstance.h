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

	//인벤토리 시스템을 위한 데이터 매니저
	UPROPERTY()
	TObjectPtr<class UQLDataManager> DataManager;
	//성별 타입과 보석 타입을 레벨 넘어갈 때 유지하기 위해서 게임 인스턴스 사용
	int GenderType; 
	int GemMatType;
};
