// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "QLGA_Attack.generated.h"

/**
 *  inputPressed�� �������� ������ �� PunchAction���� �����ؾ���. Delegate�� ���, InputPressed�� ������ ������ -> �׸��� ���� �޺� ���� ����
 */

DECLARE_DELEGATE(FOnDoubleCheckedAttack);
UCLASS()
class QUADLAND_API UQLGA_Attack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	//UQLGA_Attack();

	///** Input binding stub. */
	//virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	///** Actually activate ability, do not call this directly */
	//virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	///** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	//virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//
	//FOnDoubleCheckedAttack OnCheckedAttack;

	//UFUNCTION()
	//void OnCompleted();
	//UFUNCTION()
	//void OnInterrupted();
};
