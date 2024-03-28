// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_AttackUsingPunch.h"
#include "Character/QLCharacterPlayer.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AttackActionData/QLPunchAttackData.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GA/QLGA_Attack.h"
#include "GameFramework/GameState.h"
#include "EngineUtils.h"

UQLGA_AttackUsingPunch::UQLGA_AttackUsingPunch() : CurrentCombo(0), bHasNextPunchAttackCombo(0)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UQLGA_AttackUsingPunch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(ActorInfo->AvatarActor.Get());

	if (Player)
	{
		PunchAttackData = Player->GetPunchAttackData();
		PreferredAttackAbility = Cast<UQLGA_Attack>(TriggerEventData->OptionalObject); //약참조로 연결, 어빌리티가 살아있는 동안 이전 어빌리티도 살아있을 것이라고 일단...예상해봄 ^^!
		
		if (PreferredAttackAbility) //약포인터가 유효한지를 체크한다.
		{
			UE_LOG(LogTemp, Log, TEXT("3"));
			PreferredAttackAbility->OnCheckedAttack.BindUObject(this, &UQLGA_AttackUsingPunch::OnDoubleCheckedCallback);
		}
		/*
		현재 목표 : 연속으로 눌렀는지를 확인하기 위해서 Attack을 찾아서 Delegate 연결을 해주는 것을 목표로 한다.
		TrrigerEventData로 전달된 OptionObject 는 Parent 를 담아서 다이나믹 연결 완.
		*/
		UE_LOG(LogTemp, Log, TEXT("1"));
		PlayAttackAnimation();
	}
}

void UQLGA_AttackUsingPunch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	PunchAttackData = nullptr;
	PreferredAttackAbility = nullptr;

	CurrentCombo = 0;
	bHasNextPunchAttackCombo = false;
	PunchAttackComboTimer.Invalidate(); 
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
	//UE_LOG(LogTemp, Log, TEXT("double Click"));

	if (PunchAttackComboTimer.IsValid())
	{
		bHasNextPunchAttackCombo = true;
	}
	else
	{
		bHasNextPunchAttackCombo = false;
	}
}

void UQLGA_AttackUsingPunch::PlayAttackAnimation()
{

	UE_LOG(LogTemp, Log, TEXT("2"));

	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(CurrentActorInfo->AvatarActor.Get());

	UAnimMontage* AnimMontageUsingPunch = Player->GetAnimMontageUsingPunch();
	float AnimSpeedRate = PunchAttackData->AttackSpeedRate;
	
	//몽타주를 클라이언트 - Server를 다르게 동작하도록 한다.

	UAbilityTask_PlayMontageAndWait* AttackUsingPunchMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PunchAnimMontage"), AnimMontageUsingPunch, AnimSpeedRate , GetNextSection());
	AttackUsingPunchMontage->OnCompleted.AddDynamic(this, &UQLGA_AttackUsingPunch::OnCompleted);
	AttackUsingPunchMontage->OnInterrupted.AddDynamic(this, &UQLGA_AttackUsingPunch::OnInterrupted);

	if (PreferredAttackAbility)
	{
		AttackUsingPunchMontage->OnCompleted.AddDynamic(PreferredAttackAbility, &UQLGA_Attack::OnCompleted);
		AttackUsingPunchMontage->OnInterrupted.AddDynamic(PreferredAttackAbility, &UQLGA_Attack::OnInterrupted);
	}

	AttackUsingPunchMontage->ReadyForActivation();
	SetPunchComboCheckTimer();
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