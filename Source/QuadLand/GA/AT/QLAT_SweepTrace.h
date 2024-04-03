// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "QLAT_SweepTrace.generated.h"

/**
 * 
 */


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTraceCompleted, const FGameplayAbilityTargetDataHandle&, TargetDataHandle);

UCLASS()
class QUADLAND_API UQLAT_SweepTrace : public UAbilityTask
{
	GENERATED_BODY()

public:
	UQLAT_SweepTrace();

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "SweepTraceUsingPunch", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UQLAT_SweepTrace* CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<class AQLTA_TraceResult> TargetActorClass, FVector& SocketPos);
	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;

	UPROPERTY(BlueprintAssignable)
	FOnTraceCompleted OnCompleted;

	void SpawnAndInitTargetActor(); //생성
	void FinishSpawnning(); //초기화 마무리

	void OnCompletedCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

protected:

	UPROPERTY()
	TSubclassOf<class AQLTA_TraceResult> TargetActorClass;
	UPROPERTY()
	TObjectPtr<class AQLTA_TraceResult> TargetActor;
	FVector CurrentSocketPos;

	//동기화를 위함
	virtual void InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent) override;
};
