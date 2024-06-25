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
		GunSound = SoundCueObject.Object;
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
	
	if (AttackTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
		AttackTimerHandle.Invalidate();
		CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateInputDirectly);
		return;
	}

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

	if (WeaponStat && WeaponStat->GetCurrentAmmo() <= 0.0f)
	{
		if (IsLocallyControlled())
		{
			//ÂûÄ¬ ÂûÄ¬ ¼Ò¸®³ª±â
			InputReleased(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
		}
		return;
	}

	if (SourceASC->HasMatchingGameplayTag(CHARACTER_STATE_WIN))
	{
		if (IsLocallyControlled())
		{
			//ÂûÄ¬ ÂûÄ¬ ¼Ò¸®³ª±â
			InputReleased(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
		}
		return ;
	}

	if (IsLocallyControlled())
	{
		APlayerCameraManager* LocalCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		LocalCamera->StartCameraShake(CameraShakeClass);
	}
	
	if (HasAuthority(&CurrentActivationInfo))
	{
		Character->MakeNoise();
		Character->MulticastRPCGunAttackAnimMont();
	}

	if (IsLocallyControlled())
	{
		Character->StartRecoil();
		if (Character->GetIsShooting() == false)
		{
			Character->ServerRPCShooting();
		}
	}
	
	if (HasAuthority(&CurrentActivationInfo))
	{
		MulticastRPCShoot(Character);

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
		CueParams.Location = Character->CalPlayerLocalCameraStartPos() + Character->GetCameraForward() * Dist; //ÇöÀç´Â ÀÓ½Ã°ª ¾îÆ®¸®ºäÆ® ¼Â¿¡¼­ °¡Á®¿Ã ¿¹Á¤
		//ÇöÀç ASC¸¦ °¡Á®¿Í¼­ ExecuteGameplayCue ½ÇÇà 
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

void UQLGA_AttackUsingGunByAutonomatic::MulticastRPCStopAttack_Implementation()
{
	OnCompleted();
}

void UQLGA_AttackUsingGunByAutonomatic::MulticastRPCShoot_Implementation(AQLCharacterPlayer* Character)
{
	UGameplayStatics::PlaySoundAtLocation(Character->GetMesh(), GunSound, Character->GetWeaponMuzzlePos());
}

void UQLGA_AttackUsingGunByAutonomatic::ServerRPCStopAttack_Implementation()
{
	AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());
	Character->SetIsShooting(false);
	OnCompleted();
}


void UQLGA_AttackUsingGunByAutonomatic::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{

	AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());

	Character->SetIsShooting(false);
	Character->ReverseRecoil();
	if (ENetMode::NM_Client == Character->GetNetMode())
	{
		ServerRPCStopAttack();
	}

	OnCompleted();
}

void UQLGA_AttackUsingGunByAutonomatic::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	AttackTimerHandle.Invalidate();

	AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());

	Character->SetIsShooting(false);
	Character->ReverseRecoil();
	if (ENetMode::NM_Client == Character->GetNetMode())
	{
		ServerRPCStopAttack();
	}

	OnCompleted();
	
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}


void UQLGA_AttackUsingGunByAutonomatic::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	AttackTimerHandle.Invalidate();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}