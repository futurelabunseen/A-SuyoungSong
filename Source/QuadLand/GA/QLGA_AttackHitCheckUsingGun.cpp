// Fill out your copyright notice in the Description page of Project Settings.


#include "QLGA_AttackHitCheckUsingGun.h"
#include "TA/QLTA_LineTraceResult.h"
#include "AT/QLAT_LineTrace.h"
#include "Camera/PlayerCameraManager.h"
#include "Math/UnrealMathUtility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "GameplayTag/GamplayTags.h"
#include "GameFramework/Character.h"
#include "Interface/QLLifestoneContainerInterface.h"
#include "Character/QLCharacterPlayer.h"
#include "QuadLand.h"

UQLGA_AttackHitCheckUsingGun::UQLGA_AttackHitCheckUsingGun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	HeadDistThreshold = 30.0f;
}

void UQLGA_AttackHitCheckUsingGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	QL_GASLOG(QLLog, Log, TEXT("Gun"));
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
	if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0))
	{
		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();

		//TargetActor�� ���� ASC�� ������ ���� �±� ����
		FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
			
		const UQLAS_WeaponStat* SourceAttributeSet = SourceASC->GetSet<UQLAS_WeaponStat>();

		//Effect Handle ����

		//Gameplay Effect�� �����Ѵ�.
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect);
		// CueContextHandle -> Params ���μ� ����
		FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
		CueContextHandle.AddHitResult(HitResult);
		
		AQLCharacterBase* Character = Cast<AQLCharacterBase>(CurrentActorInfo->AvatarActor.Get());

		if (Character)
		{
			CueContextHandle.AddSourceObject(SourceAttributeSet);
		}

		if (EffectSpecHandle.IsValid()&&HitResult.GetActor())
		{
			EffectSpecHandle.Data->SetContext(CueContextHandle);
			//�߻�
			const IQLLifestoneContainerInterface* ReceivedCharacter = Cast<IQLLifestoneContainerInterface>(HitResult.GetActor()); //��ӹ��� ĳ����/���͸� �������� ���� �� ���� 
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());
			/*�� �϶� �����ؾ��ϴµ�?*/

			if (ReceivedCharacter)
			{
				
				AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(HitResult.GetActor());
				if (Player)
				{
					//�÷��̾�� TryActivateAbilitiesByTag �� ����
					FGameplayTagContainer TargetTag(CHARACTER_ATTACK_TAKENDAMAGE);
					TargetASC->TryActivateAbilitiesByTag(TargetTag);
				}
				else
				{
					//�÷��̾�� ���ʿ䰡 ���µ�, AI�� ���� ������ �� �ʿ䰡 ����.
					//AI�� SendGameplayEventToActor ����
					FGameplayEventData Payload;
					Payload.Instigator = CurrentActorInfo->AvatarActor.Get();
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitResult.GetActor(), CHARACTER_ATTACK_TAKENDAMAGE, Payload);
				}

				//Hit ��ġ ���� ��弦�� �� +10 �����ش�.
				//Ÿ�� ������ �� ��ġ�� �����´�
				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
			}
			
		}

		FGameplayCueParameters CueParam;
		CueParam.EffectContext = CueContextHandle;

		//���� ASC�� �����ͼ� ExecuteGameplayCue ���� 
		SourceASC->ExecuteGameplayCue(GAMEPLAYCUE_CHARACTER_FIREEFFECT, CueParam);
	}

	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}