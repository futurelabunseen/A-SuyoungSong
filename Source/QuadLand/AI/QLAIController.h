// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "QLAIController.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AQLAIController();
	
	void RunAI();
	void StopAI();

protected:

	virtual void OnPossess(APawn* InPawn) override;

protected:

	UPROPERTY()
	TObjectPtr<class UBlackboardData> BBAsset;

	UPROPERTY()
	TObjectPtr<class UBehaviorTree> BTAsset;
};
