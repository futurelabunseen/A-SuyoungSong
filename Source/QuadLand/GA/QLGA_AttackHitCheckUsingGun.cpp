// Fill out your copyright notice in the Description page of Project Settings.


#include "QLGA_AttackHitCheckUsingGun.h"
#include "TA/QLTA_LineTraceResult.h"
#include "AT/QLAT_LineTrace.h"

#include "Math/UnrealMathUtility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "GameplayTag/GamplayTags.h"
#include "GameFramework/Character.h"
#include "Interface/QLReceivedDamageInterface.h"

#include "QuadLand.h"

UQLGA_AttackHitCheckUsingGun::UQLGA_AttackHitCheckUsingGun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	Type = ECharacterAttackType::GunAttack;
	HeadDistThreshold = 30.0f;
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

	QL_GASLOG(QLNetLog, Log, TEXT("Current Gun Section"));

	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority)
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

			if (EffectSpecHandle.IsValid())
			{
				//�߻�
				const IQLReceivedDamageInterface* ReceivedCharacter = Cast<IQLReceivedDamageInterface>(HitResult.GetActor()); //��ӹ��� ĳ����/���͸� �������� ���� �� ���� 
				if (ReceivedCharacter)
				{

					FGameplayEventData Payload;
					Payload.EventMagnitude = static_cast<float>(Type); //Type���� ���濹��
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitResult.GetActor(), CHARACTER_ATTACK_TAKENDAMAGE, Payload);

					//Hit ��ġ ���� ��弦�� �� +10 �����ش�.
					//Ÿ�� ������ �� ��ġ�� �����´�
					ACharacter* TargetActor = Cast<ACharacter>(HitResult.GetActor());
					FVector HeadBonePos = TargetActor->GetMesh()->GetBoneLocation(TEXT("head"));

					//�Ÿ� ���� 
					FVector DistanceVector = HeadBonePos - HitResult.ImpactPoint; //���Ͱ��� �Ÿ�
					float Distance = DistanceVector.Size(); //�� ũ��
					double Damage = SourceAttributeSet->GetDamage();

					if (Distance < HeadDistThreshold)
					{
						Damage += 10.0f; //head �� ������ ������ +10
						QL_GASLOG(QLNetLog, Warning, TEXT("headshot %lf damage %lf"), Distance,Damage);
					}

					EffectSpecHandle.Data->SetSetByCallerMagnitude(DATA_STAT_DAMAGE, -Damage);
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
