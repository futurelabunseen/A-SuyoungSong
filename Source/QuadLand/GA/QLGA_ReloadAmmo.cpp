// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_ReloadAmmo.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/GamplayTags.h"
#include "Character/QLCharacterPlayer.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "QuadLand.h"

UQLGA_ReloadAmmo::UQLGA_ReloadAmmo() : ItemType(EItemType::Ammo)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
}

void UQLGA_ReloadAmmo::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
}

void UQLGA_ReloadAmmo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//ASC를 가져온다.
	//Attribute Set 을 가져와서 GetSet으로 가져온데, Ammo 값이 만빵이면 EndAbility 종료한다. 
	
	UAbilitySystemComponent* Source = GetAbilitySystemComponentFromActorInfo_Checked();
	if (Source)
	{
		QL_GASLOG(QLNetLog, Log, TEXT("1"));
		const UQLAS_WeaponStat* WeaponStat=Source->GetSet<UQLAS_WeaponStat>();
		if (!WeaponStat || WeaponStat->GetMaxAmmoCnt() <= 0.0f)
		{
			OnCompletedCallback();
			QL_GASLOG(QLNetLog, Log, TEXT("2"));
			return;
		}
	//Reload 하는 애니메이션 동작 - Player
		float AnimSpeedRate = 1.0f;
		UAbilityTask_PlayMontageAndWait* ReloadMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ReloadMontage"), ReloadAnimMontage, AnimSpeedRate);
		ReloadMontage->OnCompleted.AddDynamic(this, &UQLGA_ReloadAmmo::OnCompletedCallback);
		ReloadMontage->OnInterrupted.AddDynamic(this, &UQLGA_ReloadAmmo::OnInterruptedCallback);
		ReloadMontage->ReadyForActivation();
		QL_GASLOG(QLNetLog, Log, TEXT("1"));
	//Reload 하는 애니메이션 동작 - Mesh

	}
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(ActorInfo->AvatarActor.Get());
	if (IsLocallyControlled())
	{
		Player->ServerRPCReload();
		Player->ServerRPCRemoveItem(ItemType,Player->GetInventoryCnt(ItemType));
	}
}

void UQLGA_ReloadAmmo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_ReloadAmmo::OnCompletedCallback()
{
	//GameEffect 호출 
	UAbilitySystemComponent* Source = GetAbilitySystemComponentFromActorInfo_Checked();
	const UQLAS_WeaponStat* WeaponStat = Source->GetSet<UQLAS_WeaponStat>();
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(ReloadAmmoEffect);
	if (WeaponStat->GetMaxAmmoCnt()>0.0f&&EffectSpecHandle.IsValid())
	{
		const float CurrentAmmoCnt = WeaponStat->GetAmmoCnt();
		EffectSpecHandle.Data->SetSetByCallerMagnitude(DATA_STAT_AMMOCNT, CurrentAmmoCnt);
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle);
	}
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());
	if (IsLocallyControlled())
	{
		if (Player->GetIsReload())
		{
			Player->ServerRPCReload();
		}
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
