// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QLGemData.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLGemData : public UDataAsset
{
	GENERATED_BODY()
	
public:

	class UTexture2D* GetTexture() { return GemUITexture; }

	TSubclassOf<class AQLPlayerLifeStone> GetLifeStoneClass() { return GemClass; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	TObjectPtr<class UTexture2D> GemUITexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	TSubclassOf<class AQLPlayerLifeStone> GemClass;
	
};
