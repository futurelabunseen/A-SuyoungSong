// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_ReloadAmmo.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/GamplayTags.h"
#include "Character/QLCharacterPlayer.h"
#include "Character/QLInventoryComponent.h"
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

bool UQLGA_ReloadAmmo::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	bool Result = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	UAbilitySystemComponent* Source = GetAbilitySystemComponentFromActorInfo_Checked();
	const UQLAS_WeaponStat* WeaponStat = Source->GetSet<UQLAS_WeaponStat>();

	if (WeaponStat->GetCurrentAmmo() >= 25.0f)
	{
		return false;
	}
	return Result;
}

void UQLGA_ReloadAmmo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
		//ASC�� �����´�.
	//Attribute Set �� �����ͼ� GetSet���� �����µ�, Ammo ���� �����̸� EndAbility �����Ѵ�. 
	
	UAbilitySystemComponent* Source = GetAbilitySystemComponentFromActorInfo_Checked();
	const UQLAS_WeaponStat* WeaponStat = Source->GetSet<UQLAS_WeaponStat>();
	if (!WeaponStat || WeaponStat->GetMaxAmmoCnt() <= 0.0f)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	if (Source)
	{
		
	//Reload �ϴ� �ִϸ��̼� ���� - Player
		float AnimSpeedRate = 1.0f;
		UAbilityTask_PlayMontageAndWait* ReloadMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ReloadMontage"), ReloadAnimMontage, AnimSpeedRate);
		ReloadMontage->OnCompleted.AddDynamic(this, &UQLGA_ReloadAmmo::OnCompletedCallback);
		ReloadMontage->OnInterrupted.AddDynamic(this, &UQLGA_ReloadAmmo::OnInterruptedCallback);
		ReloadMontage->ReadyForActivation();
	//Reload �ϴ� �ִϸ��̼� ���� - Mesh

	}
	if (IsLocallyControlled()) //Ŭ���̾�Ʈ���� �˻��ϱ� ������ �̷���..üũ �ٵ� ��� RPC�ƴϰ� HasAuthority�� �ϰԵǸ�Ǵµ�...
	{
		AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(ActorInfo->AvatarActor.Get());
		QL_GASLOG(QLLog, Warning, TEXT("Begin"));

		Player->ServerRPCReload();
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
	if (WeaponStat->GetMaxAmmoCnt()>0.0f&&EffectSpecHandle.IsValid())
	{
		float CurrentAmmoCnt = CurrentAmmoCnt = WeaponStat->GetAmmoCnt() - WeaponStat->GetCurrentAmmo(); //25 - 25 - 0 -> 0..!?
		
		CurrentAmmoCnt = (CurrentAmmoCnt < WeaponStat->GetMaxAmmoCnt()) ? CurrentAmmoCnt : WeaponStat->GetMaxAmmoCnt();

		EffectSpecHandle.Data->SetSetByCallerMagnitude(DATA_STAT_AMMOCNT, WeaponStat->GetCurrentAmmo() + CurrentAmmoCnt);
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
