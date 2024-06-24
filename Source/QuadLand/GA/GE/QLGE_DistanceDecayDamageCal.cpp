// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GE/QLGE_DistanceDecayDamageCal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "GameplayTag/GamplayTags.h"
void UQLGE_DistanceDecayDamageCal::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent(); //맞은 사람이 실행되는 구문임.
	FGameplayEffectSpec Spec = ExecutionParams.GetOwningSpec();
	const FHitResult *Result = Spec.GetContext().GetHitResult();
	
	if (TargetASC)
	{
		ACharacter* Character = Cast<ACharacter>(Result->GetActor());

		if (Character)
		{
			const UQLAS_WeaponStat* WeaponStat = Cast<UQLAS_WeaponStat>(Spec.GetContext().GetSourceObject());

			if (WeaponStat == nullptr) return;

			FVector BaseBoneLocation = Character->GetMesh()->GetSocketLocation(DamageBaseBone);
			FName TargetBoneName = Result->BoneName;
			FVector TargetLocation = Character->GetMesh()->GetSocketLocation(TargetBoneName);

			float OutRadius, OutHeight;

			Character->GetCapsuleComponent()->GetScaledCapsuleSize(OutRadius, OutHeight); //캐릭터의 캡슐을 가져옴
			const float MaxDamage = WeaponStat->GetAttackDamage();  //데미지
			const float AttackDistacne = OutHeight * 2.0f; //캡슐은 액터로케이션을 기준으로 양방향으로 커짐.
			const float Distance = FMath::Clamp(FVector::Distance(BaseBoneLocation, TargetLocation), 0.0f, AttackDistacne);
			const float InvDamageRatio = 1.0f - Distance / AttackDistacne;
			float Damage = InvDamageRatio * MaxDamage;

			FGameplayCueParameters CueParams;
			CueParams.EffectContext = ExecutionParams.GetOwningSpec().GetContext(); //현재 Context같이 전달, HitTarget을 담은 Context
			CueParams.RawMagnitude = Damage; //계산된 데미지

			TargetASC->ExecuteGameplayCue(GAMEPLAYCUE_CHARACTER_DAMAGESCORE, CueParams); //데미지 스코어를 계산
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UQLAS_PlayerStat::GetDamageAttribute(), EGameplayModOp::Additive, Damage));
		}
	}
}
