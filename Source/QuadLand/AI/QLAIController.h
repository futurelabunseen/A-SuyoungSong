// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Interface/QLAIAttackInterface.h"
#include "QLAIController.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLAIController : public AAIController, public IQLAIAttackInterface
{
	GENERATED_BODY()
	
public:
	AQLAIController();
	
	void RunAI();
	void StopAI();

	virtual const APawn* GetTarget() override;
	virtual FVector GetTargetPos() override;
	virtual void SetTargetPos() override;
	void StartAI();
	bool GetCanStartForAI() { return bCanStartForAI; }
protected:

	virtual void OnPossess(APawn* InPawn) override;

	uint8 bCanStartForAI : 1;
protected:

	FVector TargetPos;

	UPROPERTY()
	TObjectPtr<class UBlackboardData> BBAsset;

	UPROPERTY()
	TObjectPtr<class UBehaviorTree> BTAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float DelayAITime = 20.0f;

	FVector TargetLocation;
};
