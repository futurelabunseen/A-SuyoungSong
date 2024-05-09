// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_BombThrower.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UQLGA_BombThrower::UQLGA_BombThrower()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UQLGA_BombThrower::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
}

void UQLGA_BombThrower::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_PlayMontageAndWait* BombMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("BombMontage"), ThrowAnimMontage, 1.0f);
	BombMontage->OnCompleted.AddDynamic(this, &UQLGA_BombThrower::OnCompletedCallback);
	BombMontage->OnInterrupted.AddDynamic(this, &UQLGA_BombThrower::OnInterruptedCallback);
	BombMontage->ReadyForActivation();
}

void UQLGA_BombThrower::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_BombThrower::OnCompletedCallback()
{
}

void UQLGA_BombThrower::OnInterruptedCallback()
{
}
