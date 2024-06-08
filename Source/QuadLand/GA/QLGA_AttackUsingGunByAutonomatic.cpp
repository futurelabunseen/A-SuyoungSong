// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_AttackUsingGunByAutonomatic.h"


#include "Character/QLCharacterPlayer.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "AT/QLAT_LineTrace.h"
#include "TA/QLTA_LineTraceResult.h"
#include "Camera/CameraShakeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTag/GamplayTags.h"
#include "Animation/AnimInstance.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "Sound/SoundCue.h"
#include "QuadLand.h"

UQLGA_AttackUsingGunByAutonomatic::UQLGA_AttackUsingGunByAutonomatic()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	static ConstructorHelpers::FObjectFinder<USoundCue> SoundCueObject(TEXT("/Script/Engine.SoundCue'/Game/MilitaryWeapSilver/Sound/Rifle/Cues/RifleA_Fire_Cue.RifleA_Fire_Cue'"));

	if (SoundCueObject.Object)
	{
		Sound = SoundCueObject.Object;
	}
}

bool UQLGA_AttackUsingGunByAutonomatic::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	bool Result = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	const UQLAS_WeaponStat* WeaponStat = SourceASC->GetSet<UQLAS_WeaponStat>();
	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

	if (AnimInstance == nullptr)
	{
		return false;
	}

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

	AQLCharacterBase* Character = Cast<AQLCharacterBase>(GetActorInfo().AvatarActor.Get());
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(Character);

	if (Player && Player->GetIsJumping())
	{
		return false;
	}

	return true;
}

void UQLGA_AttackUsingGunByAutonomatic::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (AttackTimerHandle.IsValid() == false)
	{
		GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &UQLGA_AttackUsingGunByAutonomatic::Attack, 0.1f, true);
	}

}

void UQLGA_AttackUsingGunByAutonomatic::Attack()
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	const UQLAS_WeaponStat* WeaponStat = SourceASC->GetSet<UQLAS_WeaponStat>();
	AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());
	UAnimMontage* AnimMontageUsingGun = Character->GetAnimMontage();
	UAnimInstance* AnimInstance = GetActorInfo().GetAnimInstance();

	if (WeaponStat && WeaponStat->GetCurrentAmmo() <= 0.0f)
	{
		if (IsLocallyControlled())
		{
			//찰칵 찰칵 소리나기
			InputReleased(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
		}
		return;
	}

	if (IsLocallyControlled())
	{
		APlayerCameraManager* LocalCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		LocalCamera->StartCameraShake(CameraShakeClass);
	}
	QL_GASLOG(QLLog, Warning, TEXT("Current Effect Spec"));

	float AnimSpeedRate = 1.5f;

	if (AnimInstance->Montage_IsActive(AnimMontageUsingGun) == false)
	{
		AnimInstance->Montage_Play(AnimMontageUsingGun, AnimSpeedRate);
	}

	if (IsLocallyControlled())
	{
		Character->StartRecoil();
		if (Character->GetIsShooting() == false)
		{
			Character->ServerRPCShooting();
		}
	}

	UGameplayStatics::PlaySoundAtLocation(Character,Sound,Character->GetWeaponMuzzlePos());
	
	if (HasAuthority(&CurrentActivationInfo))
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(ReduceAmmoCntEffect);

		if (EffectSpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle);
		}
	}
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
	if (SourceASC)
	{
		//SourceASC->AddTag
		FGameplayTagContainer TargetTag(CHARACTER_ATTACK_HITCHECK);
		SourceASC->TryActivateAbilitiesByTag(TargetTag, false); //Attack_HITCHECK + EQUIP 
	}

}

void UQLGA_AttackUsingGunByAutonomatic::OnCompleted()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void UQLGA_AttackUsingGunByAutonomatic::ServerRPCStopAttack_Implementation()
{
	OnCompleted();
	AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());
}


void UQLGA_AttackUsingGunByAutonomatic::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	
	AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());
	if (Character->GetIsShooting())
	{
		Character->ServerRPCShooting();
	}

	OnCompleted();
	ServerRPCStopAttack();
	Character->ReverseRecoil();
	QL_GASLOG(QLLog, Warning, TEXT("Released"));
}

void UQLGA_AttackUsingGunByAutonomatic::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	InputReleased(Handle, ActorInfo, ActivationInfo);
}


void UQLGA_AttackUsingGunByAutonomatic::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	AttackTimerHandle.Invalidate();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}