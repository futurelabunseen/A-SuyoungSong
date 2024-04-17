// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_ReloadAmmo.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/GamplayTags.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "QuadLand.h"

UQLGA_ReloadAmmo::UQLGA_ReloadAmmo()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UQLGA_ReloadAmmo::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
}

void UQLGA_ReloadAmmo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//ASC�� �����´�.
	//Attribute Set �� �����ͼ� GetSet���� �����µ�, Ammo ���� �����̸� EndAbility �����Ѵ�. 
	
	UAbilitySystemComponent* Source = GetAbilitySystemComponentFromActorInfo_Checked();
	if (Source)
	{
		const UQLAS_WeaponStat* WeaponStat=Source->GetSet<UQLAS_WeaponStat>();
		if (!WeaponStat || WeaponStat->GetAmmoCnt() == WeaponStat->GetMaxAmmoCnt())
		{
			OnCompletedCallback();
			return;
		}
	//Reload �ϴ� �ִϸ��̼� ���� - Player
		float AnimSpeedRate = 1.0f;
		UAbilityTask_PlayMontageAndWait* ReloadMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ReloadMontage"), ReloadAnimMontage, AnimSpeedRate);
		ReloadMontage->OnCompleted.AddDynamic(this, &UQLGA_ReloadAmmo::OnCompletedCallback);
		ReloadMontage->OnInterrupted.AddDynamic(this, &UQLGA_ReloadAmmo::OnInterruptedCallback);
		ReloadMontage->ReadyForActivation();
		QL_GASLOG(QLNetLog, Log, TEXT("1"));
	//Reload �ϴ� �ִϸ��̼� ���� - Weapon

	}
}

void UQLGA_ReloadAmmo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_ReloadAmmo::OnCompletedCallback()
{
	//GameEffect ȣ�� 
	UAbilitySystemComponent* Source = GetAbilitySystemComponentFromActorInfo_Checked();
	const UQLAS_WeaponStat* WeaponStat = Source->GetSet<UQLAS_WeaponStat>();
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(ReloadAmmoEffect);
	if (EffectSpecHandle.IsValid())
	{
		const float CurrentAmmoCnt = WeaponStat->GetMaxAmmoCnt() - WeaponStat->GetAmmoCnt();
		EffectSpecHandle.Data->SetSetByCallerMagnitude(DATA_STAT_AMMOCNT, CurrentAmmoCnt);
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle);
	}
	
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_ReloadAmmo::OnInterruptedCallback()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}
