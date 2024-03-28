// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_AttackHitCheck.h"
#include "AT/QLAT_SweepTrace.h"
#include "TA/QLTA_SweepTraceResult.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Character/QLCharacterPlayer.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "QuadLand.h"

UQLGA_AttackHitCheck::UQLGA_AttackHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UQLGA_AttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ResultSocket = Cast<USkeletalMeshSocket>(TriggerEventData->OptionalObject);
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(ActorInfo->AvatarActor.Get());

	if (Player)
	{
		FVector SocketPos = ResultSocket->GetSocketLocation(Player->GetMesh());
		UQLAT_SweepTrace* SweepTrace = UQLAT_SweepTrace::CreateTask(this, AQLTA_SweepTraceResult::StaticClass(), SocketPos);	
		SweepTrace->OnCompleted.AddDynamic(this, &UQLGA_AttackHitCheck::OnCompletedCallback);  //������� ���޵� ����
		SweepTrace->ReadyForActivation(); //�亯 �ö����� ���
	}
}

void UQLGA_AttackHitCheck::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ResultSocket = nullptr;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_AttackHitCheck::OnCompletedCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority)
	{
		if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0))
		{
			FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
			QL_GASLOG(QLNetLog, Log, TEXT("Current Hit Result is %s"), *(HitResult.GetActor()->GetName()));
		}
	}

	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}
