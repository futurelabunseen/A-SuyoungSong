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
		FVector Center = EffectContextHandle.GetOrigin(); //시작지점

		AActor* Actor = TargetASC->GetAvatarActor();
		//Max값
		const float MaxDamage = WeaponStat->GetMaxDamage() + AddBombDamage;
		const float AttackDistacne = WeaponStat->GetAttackDistance() + AddBombRadius;
		const float Distance = FMath::Clamp(FVector::Distance(Center, Actor->GetActorLocation()), 0.0f, AttackDistacne);
		const float InvDamageRatio = 1.0f - Distance / AttackDistacne;

		float Damage = InvDamageRatio * MaxDamage;
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UQLAS_PlayerStat::GetDamageAttribute(), EGameplayModOp::Additive, Damage));
	}
}
