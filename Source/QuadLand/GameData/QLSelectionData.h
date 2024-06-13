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

	class UMaterialInterface* GetMaterial(int Idx);
	class UTexture2D* GetTexture(int Idx);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	TArray<TSubclassOf<class AQLCharacterPlayer>> GenderPawn; //���߿� TMap�� ����ؼ� ����Ŭ���� - ����Ŭ������ �ΰų�, �������̽��� ��� ����

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Color)
	TArray<TObjectPtr<class UQLGemData>> GemData; //���߿� TMap�� ����ؼ� ����Ŭ���� - ����Ŭ������ �ΰų�, �������̽��� ��� ����

};
