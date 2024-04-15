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

			//TargetActor에 대한 ASC를 가져온 다음 태그 부착
			FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
			
			const UQLAS_WeaponStat* SourceAttributeSet = SourceASC->GetSet<UQLAS_WeaponStat>();

			//Effect Handle 설정

			//Gameplay Effect를 실행한다.
			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect);

			if (EffectSpecHandle.IsValid())
			{
				//발사
				const IQLReceivedDamageInterface* ReceivedCharacter = Cast<IQLReceivedDamageInterface>(HitResult.GetActor()); //상속받은 캐릭터/몬스터만 데미지를 받을 수 있음 
				if (ReceivedCharacter)
				{

					FGameplayEventData Payload;
					Payload.EventMagnitude = static_cast<float>(Type); //Type으로 변경예정
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitResult.GetActor(), CHARACTER_ATTACK_TAKENDAMAGE, Payload);

					//Hit 위치 판정 헤드샷일 때 +10 더해준다.
					//타겟 액터의 본 위치를 가져온다
					ACharacter* TargetActor = Cast<ACharacter>(HitResult.GetActor());
					FVector HeadBonePos = TargetActor->GetMesh()->GetBoneLocation(TEXT("head"));

					//거리 판정 
					FVector DistanceVector = HeadBonePos - HitResult.ImpactPoint; //벡터간의 거리
					float Distance = DistanceVector.Size(); //그 크기
					double Damage = SourceAttributeSet->GetDamage();

					if (Distance < HeadDistThreshold)
					{
						Damage += 10.0f; //head 와 가까우면 데미지 +10
						QL_GASLOG(QLNetLog, Warning, TEXT("headshot %lf damage %lf"), Distance,Damage);
					}

					EffectSpecHandle.Data->SetSetByCallerMagnitude(DATA_STAT_DAMAGE, -Damage);
					ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
				}
			}

			/*총 일때 설정해야하는데?*/
			// CueContextHandle -> Params 감싸서 전달
			FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
			CueContextHandle.AddHitResult(HitResult);
			FGameplayCueParameters CueParam;
			CueParam.EffectContext = CueContextHandle;

			//현재 ASC를 가져와서 ExecuteGameplayCue 실행 
			SourceASC->ExecuteGameplayCue(GAMEPLAYCUE_CHARACTER_DAMAGEEFFECT, CueParam);
		}
	}

	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}
