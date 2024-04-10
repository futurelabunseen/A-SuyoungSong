// Fill out your copyright notice in the Description page of Project Settings.


#include "QLGA_AttackHitCheckUsingGun.h"
#include "TA/QLTA_LineTraceResult.h"
#include "AT/QLAT_LineTrace.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "GameplayTag/GamplayTags.h"
#include "Interface/QLReceivedDamageInterface.h"

#include "QuadLand.h"

UQLGA_AttackHitCheckUsingGun::UQLGA_AttackHitCheckUsingGun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UQLGA_AttackHitCheckUsingGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UQLAT_LineTrace* AttackLineTrace = UQLAT_LineTrace::CreateTask(this, AQLTA_LineTraceResult::StaticClass());
	AttackLineTrace->OnCompleted.AddDynamic(this, &UQLGA_AttackHitCheckUsingGun::OnCompletedCallback);
	AttackLineTrace->ReadyForActivation();
}

void UQLGA_AttackHitCheckUsingGun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void UQLGA_AttackHitCheckUsingGun::OnCompletedCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority)
	{
		if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0))
		{
			UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();

			FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
			const UQLAS_WeaponStat* SourceAttributeSet = SourceASC->GetSet<UQLAS_WeaponStat>();

			//Effect Handle ����

			//Gameplay Effect�� �����Ѵ�.
			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect);

			if (EffectSpecHandle.IsValid())
			{
				//�߻�
				const IQLReceivedDamageInterface* TargetActor = Cast<IQLReceivedDamageInterface>(HitResult.GetActor());
				if (TargetActor)
				{
					EffectSpecHandle.Data->SetSetByCallerMagnitude(DATA_STAT_DAMAGE, -SourceAttributeSet->GetDamage());
					ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
				}
			}

			/*�� �϶� �����ؾ��ϴµ�?*/
			// CueContextHandle -> Params ���μ� ����
			FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
			CueContextHandle.AddHitResult(HitResult);
			FGameplayCueParameters CueParam;
			CueParam.EffectContext = CueContextHandle;

			//���� ASC�� �����ͼ� ExecuteGameplayCue ���� 
			SourceASC->ExecuteGameplayCue(GAMEPLAYCUE_CHARACTER_DAMAGEEFFECT, CueParam);
		}
	}

	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}
