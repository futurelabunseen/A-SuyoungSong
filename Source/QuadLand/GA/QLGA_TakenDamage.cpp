// Fill out your copyright notice in the Description page of Project Settings.

#include "QLGA_TakenDamage.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTag/GamplayTags.h"
#include "QuadLand.h"
UQLGA_TakenDamage::UQLGA_TakenDamage()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//태그 부착 -> Required Tag : 공격받았을 때 호출
}

void UQLGA_TakenDamage::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//CommitAbility(Handle, ActorInfo, ActivationInfo); //CommitAbility

	QL_GASLOG(QLNetLog, Log, TEXT("Current Punch Section"));

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	
	if (SourceASC->HasMatchingGameplayTag(CHARACTER_EQUIP_NON))
	{
		Tag = CHARACTER_EQUIP_NON;
	}
	else
	{
		Tag = CHARACTER_EQUIP_GUNTYPEA;
	}

	float AnimSpeedRate = 1.0f;
	UAbilityTask_PlayMontageAndWait* AttackUsingPunchMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("TakenDamageActor"), DamageMontage[Tag], AnimSpeedRate);
	AttackUsingPunchMontage->OnCompleted.AddDynamic(this, &UQLGA_TakenDamage::OnCompletedCallback);
	AttackUsingPunchMontage->OnInterrupted.AddDynamic(this, &UQLGA_TakenDamage::OnInterruptedCallback);
	AttackUsingPunchMontage->ReadyForActivation();

}

void UQLGA_TakenDamage::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_TakenDamage::OnCompletedCallback()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_TakenDamage::OnInterruptedCallback()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}
