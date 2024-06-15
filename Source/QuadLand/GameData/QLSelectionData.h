// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QLSelectionData.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLSelectionData : public UDataAsset
{
	GENERATED_BODY()
	
public:

	TSubclassOf<class AQLPlayerLifeStone> GetLifeStoneClass(int Idx);
	class UTexture2D* GetTexture(int Idx);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	TArray<TSubclassOf<class AQLCharacterPlayer>> GenderPawn; //나중엔 TMap을 사용해서 상위클래스 - 하위클래스로 두거나, 인터페이스로 묶어서 관리

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Color)
	TArray<TObjectPtr<class UQLGemData>> GemData; //나중엔 TMap을 사용해서 상위클래스 - 하위클래스로 두거나, 인터페이스로 묶어서 관리

};
