// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QLNickname.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLNickname : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nickname")
	TArray<FString> Nickname;
	
};
