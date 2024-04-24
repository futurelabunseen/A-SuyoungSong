// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QLItemDataset.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLItemDataset : public UDataAsset
{
	GENERATED_BODY()
	
public:

	class UQLItemData* GetItem(int id);

protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<TObjectPtr<class UQLItemData>> ItemList;

};
