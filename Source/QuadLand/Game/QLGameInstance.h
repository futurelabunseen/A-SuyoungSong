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

	/** virtual function to allow custom GameInstances an opportunity to set up what it needs */
	virtual void Init();

	UPROPERTY()
	TObjectPtr<class UQLDataManager> DataManager;
	int GenderType;
	int GemMatType;
};
