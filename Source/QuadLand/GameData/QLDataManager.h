// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "QLDataManager.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLDataManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UQLDataManager();

	UFUNCTION(BlueprintCallable)
	void PrintItemList();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY()
	TObjectPtr<class UQLItemDataset> ItemDataManager; //���߿� TMap�� ����ؼ� ����Ŭ���� - ����Ŭ������ �ΰų�, �������̽��� ��� ����

};

