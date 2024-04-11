// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_AttackUsingPunch.h"
#include "Character/QLCharacterPlayer.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "AttackActionData/QLPunchAttackData.h"
#include "GameFramework/GameState.h"
#include "EngineUtils.h"

UQLGA_AttackUsingPunch::UQLGA_AttackUsingPunch() : CurrentCombo(0), bHasNextPunchAttackCombo(0)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UQLGA_AttackUsingPunch::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (PunchAttackComboTimer.IsValid())
	{
		bHasNextPunchAttackCombo = true;
	}
	else
	{
		bHasNextPunchAttackCombo = false;
	}
}

void UQLGA_AttackUsingPunch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(ActorInfo->AvatarActor.Get());

	if (Player)
	{
		PunchAttackData = Player->GetPunchAttackData();
		/*
		���� ��ǥ : �������� ���������� Ȯ���ϱ� ���ؼ� Attack�� ã�Ƽ� Delegate ������ ���ִ� ���� ��ǥ�� �Ѵ�.
		TrrigerEventData�� ���޵� OptionObject �� Parent �� ��Ƽ� ���̳��� ���� ��.
		*/
		UE_LOG(LogTemp, Log, TEXT("1"));
		PlayAttackAnimation();
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
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, PunchAttackData->MaxFrameCount); //�������� �̵�
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

void UQLGA_AttackUsingPunch::PlayAttackAnimation()
{
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(CurrentActorInfo->AvatarActor.Get());

	UAnimMontage* AnimMontageUsingPunch = Player->GetAnimMontage();
	float AnimSpeedRate = PunchAttackData->AttackSpeedRate;
	
	//��Ÿ�ָ� Ŭ���̾�Ʈ - Server�� �ٸ��� �����ϵ��� �Ѵ�.

	UAbilityTask_PlayMontageAndWait* AttackUsingPunchMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PunchAnimMontage"), AnimMontageUsingPunch, AnimSpeedRate , GetNextSection());
	AttackUsingPunchMontage->OnCompleted.AddDynamic(this, &UQLGA_AttackUsingPunch::OnCompleted);
	AttackUsingPunchMontage->OnInterrupted.AddDynamic(this, &UQLGA_AttackUsingPunch::OnInterrupted);
	AttackUsingPunchMontage->ReadyForActivation();
	SetPunchComboCheckTimer();
}

void UQLGA_AttackUsingPunch::SetPunchComboCheckTimer()
{
	//���� CurrentCombo ���� ������ Ȯ��
	int32 ComboIndex = CurrentCombo - 1; //������ �ε����� �ϳ� ����
	ensure(PunchAttackData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	const float AttackSpeedRate = PunchAttackData->AttackSpeedRate;
	float ComboEffectiveTime = (PunchAttackData->EffectiveFrameCount[ComboIndex] / PunchAttackData->FrameRate) / AttackSpeedRate;

	if (ComboEffectiveTime > 0.0f)
	{
		//���� �׼����� ����
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
		SetPunchComboCheckTimer(); //�ΰ��ۿ� ������ ���� ��ȹ�� ���ؼ� �ϴ� �ɾ��.
		bHasNextPunchAttackCombo = false;
	}

}