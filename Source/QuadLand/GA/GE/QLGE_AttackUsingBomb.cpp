// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GE/QLGE_AttackUsingBomb.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "AttributeSet/QLAS_WeaponStat.h"


void UQLGE_AttackUsingBomb::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//필요한 데이터 ExecutionParams 있음
	//최종 데이터 OutExecutionOutput 삽입

	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	FGameplayEffectSpec Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	const UQLAS_WeaponStat* WeaponStat = Cast<UQLAS_WeaponStat>(EffectContextHandle.GetSourceObject());

	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if (TargetASC)
	{
		FVector Center = EffectContextHandle.GetOrigin(); //Bomb이 부딪힌 지점
		AActor* Actor = TargetASC->GetAvatarActor(); //데미지 받을 액터들
		//Max값
		const float MaxDamage = WeaponStat->GetMaxDamage() + AddBombDamage;
		const float AttackDistacne = WeaponStat->GetAttackDistance() + AddBombRadius;
		const float Distance = FMath::Clamp(FVector::Distance(Center, Actor->GetActorLocation()), 0.0f, AttackDistacne);
		//거리별 점수값 가져온다.
		const float InvDamageRatio = 1.0f - Distance / AttackDistacne; //가까울 수록 더 높은 점수를 위해 1.0을 빼줌

		float Damage = InvDamageRatio * MaxDamage; //MaxDamage값을 곱해서 해당 액터에게 전달
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UQLAS_PlayerStat::GetDamageAttribute(), EGameplayModOp::Additive, Damage));
	}
}
