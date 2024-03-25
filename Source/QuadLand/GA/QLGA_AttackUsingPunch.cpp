// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_AttackUsingPunch.h"
#include "Character/QLCharacterPlayer.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AttackActionData/QLPunchAttackData.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GA/QLGA_Attack.h"

UQLGA_AttackUsingPunch::UQLGA_AttackUsingPunch() : bHasNextPunchAttackCombo(0), CurrentCombo(0)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UQLGA_AttackUsingPunch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(ActorInfo->AvatarActor.Get());

	UE_LOG(LogTemp, Log, TEXT("QLGA_AttackUsingPunch Class is running 1"));

	if (Player)
	{
		UAnimMontage* AnimMontageUsingPunch = Player->GetAnimMontageUsingPunch();
		PunchAttackData = Player->GetPunchAttackData();
		float AnimSpeedRate = PunchAttackData->AttackSpeedRate;
		UQLGA_Attack* AttackParent=Cast<UQLGA_Attack>(TriggerEventData->OptionalObject);

		/*
		현재 목표 : 연속으로 눌렀는지를 확인하기 위해서 Attack을 찾아서 Delegate 연결을 해주는 것을 목표로 한다.

		*/
		UAbilityTask_PlayMontageAndWait* AttackUsingPunchMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PunchAnimMontage"), AnimMontageUsingPunch, AnimSpeedRate , GetNextSection());
		AttackUsingPunchMontage->OnCompleted.AddDynamic(this, &UQLGA_AttackUsingPunch::OnCompleted);
		AttackUsingPunchMontage->OnInterrupted.AddDynamic(this, &UQLGA_AttackUsingPunch::OnInterrupted);

		if (AttackParent)
		{
			AttackParent->OnCheckedAttack.BindUObject(this, &UQLGA_AttackUsingPunch::OnDoubleCheckedCallback);
			AttackUsingPunchMontage->OnCompleted.AddDynamic(AttackParent, &UQLGA_Attack::OnCompleted);
			AttackUsingPunchMontage->OnCompleted.AddDynamic(AttackParent, &UQLGA_Attack::OnInterrupted);
		}
	

		AttackUsingPunchMontage->ReadyForActivation();
		SetPunchComboCheckTimer();
	}
}

void UQLGA_AttackUsingPunch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	PunchAttackData = nullptr;
	CurrentCombo = 0;
	bHasNextPunchAttackCombo = false;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FName UQLGA_AttackUsingPunch::GetNextSection()
{
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, PunchAttackData->MaxFrameCount); //다음으로 이동
	FName NextPunchSection = *FString::Printf(TEXT("%s%d"), *PunchAttackData->MontageSectionNamePrefix, CurrentCombo);

	return NextPunchSection;
}

void UQLGA_AttackUsingPunch::OnCompleted()
{
	UE_LOG(LogTemp, Log, TEXT("Anim is Ended"));

	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_AttackUsingPunch::OnInterrupted()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_AttackUsingPunch::OnDoubleCheckedCallback()
{
	if (PunchAttackComboTimer.IsValid())
	{
		bHasNextPunchAttackCombo = true;
	}
	else
	{
		bHasNextPunchAttackCombo = false;
		UE_LOG(LogTemp, Log, TEXT("double Click"));
	}
}


void UQLGA_AttackUsingPunch::SetPunchComboCheckTimer()
{
	//현재 CurrentCombo 값이 몇인지 확인
	int32 ComboIndex = CurrentCombo - 1; //실질적 인덱스는 하나 작음
	ensure(PunchAttackData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	const float AttackSpeedRate = PunchAttackData->AttackSpeedRate;
	float ComboEffectiveTime = (PunchAttackData->EffectiveFrameCount[ComboIndex] / PunchAttackData->FrameRate) / AttackSpeedRate;

	if (ComboEffectiveTime > 0.0f)
	{
		//다음 액션으로 수행
		GetWorld()->GetTimerManager().SetTimer(PunchAttackComboTimer, this,
			&UQLGA_AttackUsingPunch::PunchAttackComboCheck, ComboEffectiveTime, false);
	}

}

void UQLGA_AttackUsingPunch::PunchAttackComboCheck()
{
	PunchAttackComboTimer.Invalidate();

	if (bHasNextPunchAttackCombo)
	{	
		MontageJumpToSection(GetNextSection());
		SetPunchComboCheckTimer(); //두개밖에 없지만 다음 기획을 위해서 일단 걸어둠.
		bHasNextPunchAttackCombo = false;
	}

}