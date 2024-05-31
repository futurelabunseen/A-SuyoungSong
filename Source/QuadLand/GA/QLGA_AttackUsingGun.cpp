// Fill out your copyright notice in the Description page of Project Settings.


#include "QLGA_AttackUsingGun.h"
#include "Character/QLCharacterPlayer.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AT/QLAT_LineTrace.h"
#include "TA/QLTA_LineTraceResult.h"
#include "Camera/CameraShakeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTag/GamplayTags.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "QuadLand.h"

UQLGA_AttackUsingGun::UQLGA_AttackUsingGun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UQLGA_AttackUsingGun::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
}
bool UQLGA_AttackUsingGun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	bool Result = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	//Ammo Cnt 개수를 체크한다 만약 0이라면, 어빌리티를 실행하지 않고 종료한다.
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	const UQLAS_WeaponStat* WeaponStat = SourceASC->GetSet<UQLAS_WeaponStat>();

	if (SourceASC->HasMatchingGameplayTag(CHARACTER_STATE_RUN))
	{
		return false;
	}

	if (WeaponStat && WeaponStat->GetCurrentAmmo() <= 0.0f)
	{
		return false;
	}

	if (CameraShakeClass == nullptr)
	{
		return false;
	}

	return Result;
}

void UQLGA_AttackUsingGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//총을 쏜다면 플레이어를 회전 시킨다.
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(CurrentActorInfo->AvatarActor.Get());

	if (IsLocallyControlled())
	{
		APlayerCameraManager* LocalCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		LocalCamera->StartCameraShake(CameraShakeClass);
	}

	UAnimMontage* AnimMontageUsingGun = Player->GetAnimMontage();
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	const UQLAS_WeaponStat* WeaponStat = SourceASC->GetSet<UQLAS_WeaponStat>();

	float AnimSpeedRate = 1.0f;

	//몽타주를 클라이언트 - Server를 다르게 동작하도록 한다.
	UAbilityTask_PlayMontageAndWait* AttackUsingGunMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("GunAnimMontage"), AnimMontageUsingGun, AnimSpeedRate);
	AttackUsingGunMontage->OnCompleted.AddDynamic(this, &UQLGA_AttackUsingGun::OnCompletedCallback);
	AttackUsingGunMontage->OnInterrupted.AddDynamic(this, &UQLGA_AttackUsingGun::OnInterruptedCallback);
	
	AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());
	if (IsLocallyControlled())
	{
		if (WeaponStat->GetCurrentAmmo() <= 0.0f)
		{
			OnCompletedCallback();
			return;
		}
		//클라이언트로부터 입력 들어옴 서버 호출
		Character->ServerRPCShooting();
	}

	/*
	여기서 발동 -> 총알 횟수 주는 Effect수행
	*/
	//Gameplay Effect를 실행한다.
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(ReduceAmmoCntEffect);

	if (EffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle);

		//GAMEPLAYCUE_EFFECT_TRACEBYGUN
		if (WeaponStat)
		{
			FGameplayCueParameters CueParams;
			CueParams.Instigator = Character;
			float Dist = WeaponStat->GetAttackDistance();
			CueParams.Location = Character->CalPlayerLocalCameraStartPos() + Character->GetCameraForward() * Dist; //현재는 임시값 어트리뷰트 셋에서 가져올 예정
			//현재 ASC를 가져와서 ExecuteGameplayCue 실행 
			SourceASC->ExecuteGameplayCue(GAMEPLAYCUE_EFFECT_TRACEBYGUN, CueParams);
		}

	}
	if (SourceASC)
	{
		//SourceASC->AddTag
		FGameplayTagContainer TargetTag(CHARACTER_ATTACK_HITCHECK);
		SourceASC->TryActivateAbilitiesByTag(TargetTag,false); //Attack_HITCHECK + EQUIP 
	}
	AttackUsingGunMontage->ReadyForActivation();
}

void UQLGA_AttackUsingGun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_AttackUsingGun::OnCompletedCallback()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());
	if (IsLocallyControlled())
	{
		if (Character->GetIsShooting())
		{
			Character->ServerRPCShooting();
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_AttackUsingGun::OnInterruptedCallback()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}
