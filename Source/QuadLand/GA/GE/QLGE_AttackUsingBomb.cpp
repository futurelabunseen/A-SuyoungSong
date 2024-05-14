// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GE/QLGE_AttackUsingBomb.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "GameData/QLDataManager.h"
#include "GameData/QLWeaponStat.h"
void UQLGE_AttackUsingBomb::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//필요한 데이터 ExecutionParams 있음
	//최종 데이터 OutExecutionOutput 삽입

	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	FGameplayEffectSpec Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	UE_LOG(LogTemp, Log, TEXT("AttackUsingBomb 1"));
	if (TargetASC)
	{
		
		FVector Center = EffectContextHandle.GetOrigin(); //시작지점
		
		AActor* Actor = TargetASC->GetAvatarActor();

		UE_LOG(LogTemp, Log, TEXT("AttackUsingBomb 2 %s"),*Actor->GetName());
		//Max값
		//UQLDataManager* DataManager = GetWorld()->GetSubsystem<UQLDataManager>();

		//if (DataManager)
		//{
		//	UQLWeaponStat* Stat = Cast<UQLWeaponStat>(DataManager->GetItem(EItemType::Bomb));

		//	const float MaxDamage = Stat->Damage;
		//	const float MaxRange = Stat->AttackDist;
		//	const float Distance = FMath::Clamp(FVector::Distance(Center, Actor->GetActorLocation()), 0.0f, MaxRange);
		//	const float InvDamageRatio = 1.0f - Distance / MaxRange;

		//	float Damage = InvDamageRatio * MaxDamage;

		//	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UQLAS_PlayerStat::GetDamageAttribute(), EGameplayModOp::Additive, Damage));
		//}
	}
}
