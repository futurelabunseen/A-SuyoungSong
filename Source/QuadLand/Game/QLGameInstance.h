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
	
	void SetGenderType(int InGenderType) { GenderType = InGenderType; }
	void SetGemType(int MatType) { GemMatType = MatType; }


	int GetGenderType() { return GenderType; }
	int GetGemMatType() { return GemMatType; }
protected:

	int GenderType;
	int GemMatType;
};
