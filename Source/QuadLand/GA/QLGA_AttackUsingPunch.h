// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "QLGA_AttackUsingPunch.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLGA_AttackUsingPunch : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UQLGA_AttackUsingPunch();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


	void SetPunchComboCheckTimer();
	void PunchAttackComboCheck();
	FName GetNextSection();

protected:
	UFUNCTION()
	void OnCompleted();
	UFUNCTION()
	void OnInterrupted();

	//Attack Check에 필요한 요소 
	int32 CurrentCombo;
	int8 bHasNextPunchAttackCombo : 1;
	FTimerHandle PunchAttackComboTimer;
	UPROPERTY()
	TObjectPtr<class UQLPunchAttackData> PunchAttackData;

	void OnDoubleCheckedCallback();
};
