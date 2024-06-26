// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_AttackHitCheck.h"
#include "AT/QLAT_SweepTrace.h"
#include "TA/QLTA_TraceResult.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Character/QLCharacterPlayer.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "GameplayTag/GamplayTags.h"
#include "QuadLand.h"

UQLGA_AttackHitCheck::UQLGA_AttackHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UQLGA_AttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (TriggerEventData == nullptr)
	{
		bool bReplicateEndAbility = true;
		bool bWasCancelled = false;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}
	if (TriggerEventData->OptionalObject == nullptr)
	{
		bool bReplicateEndAbility = true;
		bool bWasCancelled = false;
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}

	ResultSocket = Cast<USkeletalMeshSocket>(TriggerEventData->OptionalObject);
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(ActorInfo->AvatarActor.Get());

	if (Player)
	{
		FVector SocketPos = ResultSocket->GetSocketLocation(Player->GetMesh());
		UQLAT_SweepTrace* SweepTrace = UQLAT_SweepTrace::CreateTask(this, AQLTA_TraceResult::StaticClass(), SocketPos);	
		SweepTrace->OnCompleted.AddDynamic(this, &UQLGA_AttackHitCheck::OnCompletedCallback);  //결과값이 전달될 예정
		
		SweepTrace->ReadyForActivation(); //답변 올때까지 대기
	}
}

void UQLGA_AttackHitCheck::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ResultSocket = nullptr;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UQLGA_AttackHitCheck::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{

	bool bResult = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	
	if (bResult == false)
	{
		return false;
	}
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();

	if (SourceASC == nullptr)
	{
		return false;
	}

	if (SourceASC->HasMatchingGameplayTag(CHARACTER_EQUIP_GUNTYPEA))
	{
		return false;
	}

	return true;
}

void UQLGA_AttackHitCheck::OnCompletedCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0))
	{
		FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);

		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();

		if (SourceASC)
		{
			const UQLAS_WeaponStat* SourceAttributeSet = SourceASC->GetSet<UQLAS_WeaponStat>();
			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect);

			AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(HitResult.GetActor());
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());
			//플레이어는 TryActivateAbilitiesByTag 로 실행
			FGameplayTagContainer TargetTag(CHARACTER_ATTACK_TAKENDAMAGE);

			if (Player)
			{	
				TargetASC->TryActivateAbilitiesByTag(TargetTag);
			}
			else
			{
				//플레이어는 알필요가 없는데, AI는 누가 때렸는지 알 필요 있음.
				//AI는 SendGameplayEventToActor 실행
				FGameplayEventData Payload;
				Payload.Instigator = CurrentActorInfo->AvatarActor.Get();
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitResult.GetActor(), CHARACTER_ATTACK_TAKENDAMAGE, Payload);
			}
			
			if (TargetASC && SourceAttributeSet)
			{
				if (EffectSpecHandle.IsValid())
				{
					EffectSpecHandle.Data->SetSetByCallerMagnitude(DATA_STAT_DAMAGE, SourceAttributeSet->GetAttackDamage());
					ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
				}

				// CueContextHandle -> Params 감싸서 전달
				FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
				CueContextHandle.AddHitResult(HitResult);
				FGameplayCueParameters CueParam;
				CueParam.Location = HitResult.Location;
				CueParam.EffectContext = CueContextHandle;

				//현재 ASC를 가져와서 ExecuteGameplayCue 실행 
				SourceASC->ExecuteGameplayCue(GAMEPLAYCUE_CHARACTER_DAMAGEEFFECT, CueParam);
			}
		}

	}

	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}
