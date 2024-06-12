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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	TArray<TSoftObjectPtr<class USkeletalMesh>> GenderMeshData; //���߿� TMap�� ����ؼ� ����Ŭ���� - ����Ŭ������ �ΰų�, �������̽��� ��� ����


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Color)
	TArray<TObjectPtr<class UQLGemData>> GemData; //���߿� TMap�� ����ؼ� ����Ŭ���� - ����Ŭ������ �ΰų�, �������̽��� ��� ����

};
