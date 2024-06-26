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

	if (Source->HasAnyMatchingGameplayTags(StopTag))
	{
		return false;
	}	
	
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(ActorInfo->AvatarActor.Get());
	if (Player && Player->GetIsJumping()) //점프할 때 총을 쏠 수 없다.
	{
		return false;
	}

	return Result;
}

void UQLGA_ReloadAmmo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
		//ASC를 가져온다.
	//Attribute Set 을 가져와서 GetSet으로 가져온데, Ammo 값이 만빵이면 EndAbility 종료한다. 

	UAbilitySystemComponent* Source = GetAbilitySystemComponentFromActorInfo_Checked();
	const UQLAS_WeaponStat* WeaponStat = Source->GetSet<UQLAS_WeaponStat>();
	if (!WeaponStat || WeaponStat->GetMaxAmmoCnt() <= 0.0f)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	if (Source)
	{
		//Reload 하는 애니메이션 동작 - Player
		float AnimSpeedRate = 1.0f;
		UAbilityTask_PlayMontageAndWait* ReloadMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ReloadMontage"), ReloadAnimMontage, AnimSpeedRate);
		ReloadMontage->OnCompleted.AddDynamic(this, &UQLGA_ReloadAmmo::OnCompletedCallback);
		ReloadMontage->OnInterrupted.AddDynamic(this, &UQLGA_ReloadAmmo::OnInterruptedCallback);
		ReloadMontage->ReadyForActivation();
		//Reload 하는 애니메이션 동작 - Mesh

	}

	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(ActorInfo->AvatarActor.Get());
	Player->StopAim();

	if (IsLocallyControlled()) //클라이언트에서 검사하기 때문에 이렇게..체크 근데 사실 RPC아니고 HasAuthority로 하게되면되는데...
	{
		Player->ServerRPCReload();
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

	if (WeaponStat->GetMaxAmmoCnt()>0.0f&&EffectSpecHandle.IsValid()) //현재 총알 최대개수가 0보다 크고, 이펙트 핸들러가 유효하다면,
	{
		float CurrentAmmoCnt = WeaponStat->GetAmmoCnt() - WeaponStat->GetCurrentAmmo(); //장전에 필요한 개수 = 장전 최대 개수(25) - 현재 이미 장정된 총알 개수(N)
		
		CurrentAmmoCnt = (CurrentAmmoCnt < WeaponStat->GetMaxAmmoCnt()) ? CurrentAmmoCnt : WeaponStat->GetMaxAmmoCnt(); 
		//장전에 필요한 총알 개수가 현재 가지고 있는 총알 값보다 크다면, 장전에 필요한 개수만 장전
		//그렇지 않다면, 최대 장전 개수 가져옴

		EffectSpecHandle.Data->SetSetByCallerMagnitude(DATA_STAT_AMMOCNT, WeaponStat->GetCurrentAmmo() + CurrentAmmoCnt); //현재 개수만큼 이펙트 실행
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
