// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "QLAT_LineTrace.generated.h"

/**
 * 
 */


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLineTraceOnCompleted, const FGameplayAbilityTargetDataHandle&, TargetDataHandle);

UCLASS()
class QUADLAND_API UQLAT_LineTrace : public UAbilityTask
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "LineTraceUsingGun", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UQLAT_LineTrace* CreateTask(UGameplayAbility* OwningAbility,TSubclassOf<class AQLTA_LineTraceResult> InTargetActorClass);
	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;

	UPROPERTY(BlueprintAssignable)
	FLineTraceOnCompleted OnCompleted;

	UFUNCTION()
	void OnCompletedCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	void SpawnAndInitTargetActor();
	void FinishSpawnning();

protected:
	UPROPERTY()
	TSubclassOf<class AQLTA_LineTraceResult> TargetActorClass;
	UPROPERTY()
	TObjectPtr<class AQLTA_LineTraceResult> TargetActor;
};
