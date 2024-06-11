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
#include "Interface/QLAIAttackInterface.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "Sound/SoundCue.h"
#include "QuadLand.h"

UQLGA_AttackUsingGun::UQLGA_AttackUsingGun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	static ConstructorHelpers::FObjectFinder<USoundCue> SoundCueObject(TEXT("/Script/Engine.SoundCue'/Game/MilitaryWeapSilver/Sound/Rifle/Cues/RifleA_Fire_Cue.RifleA_Fire_Cue'"));

	if (SoundCueObject.Object)
	{
		Sound = SoundCueObject.Object;
	}
}

void UQLGA_AttackUsingGun::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
}
bool UQLGA_AttackUsingGun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	bool Result = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	//Ammo Cnt ������ üũ�Ѵ� ���� 0�̶��, �����Ƽ�� �������� �ʰ� �����Ѵ�.
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
	AQLCharacterBase* Character = Cast<AQLCharacterBase>(GetActorInfo().AvatarActor.Get());
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(Character);

	if (Player && Player->GetIsJumping())
	{
		return false;
	}
	return Result;
}

void UQLGA_AttackUsingGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//���� ��ٸ� �÷��̾ ȸ�� ��Ų��.
	AQLCharacterBase* Character = Cast<AQLCharacterBase>(GetActorInfo().AvatarActor.Get());
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(Character);

	if (IsLocallyControlled())
	{
		if (Player)
		{
			APlayerCameraManager* LocalCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
			LocalCamera->StartCameraShake(CameraShakeClass);
		}
	}

	if (HasAuthority(&ActivationInfo))
	{
		Character->MakeNoise();
	}
	UAnimMontage* AnimMontageUsingGun = Character->GetAnimMontage();
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	const UQLAS_WeaponStat* WeaponStat = SourceASC->GetSet<UQLAS_WeaponStat>();

	float AnimSpeedRate = 1.0f;

	//��Ÿ�ָ� Ŭ���̾�Ʈ - Server�� �ٸ��� �����ϵ��� �Ѵ�.
	UAbilityTask_PlayMontageAndWait* AttackUsingGunMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("GunAnimMontage"), AnimMontageUsingGun, AnimSpeedRate);
	AttackUsingGunMontage->OnCompleted.AddDynamic(this, &UQLGA_AttackUsingGun::OnCompletedCallback);
	AttackUsingGunMontage->OnInterrupted.AddDynamic(this, &UQLGA_AttackUsingGun::OnInterruptedCallback);

	if (IsLocallyControlled())
	{
		if (WeaponStat->GetCurrentAmmo() <= 0.0f)
		{
			OnCompletedCallback();
			return;
		}
		//Ŭ���̾�Ʈ�κ��� �Է� ���� ���� ȣ��
		Character->ServerRPCShooting();
	}

	///*
	//���⼭ �ߵ� -> �Ѿ� Ƚ�� �ִ� Effect����
	//*/
	////Gameplay Effect�� �����Ѵ�.


	UGameplayStatics::PlaySoundAtLocation(Character, Sound, Character->GetWeaponMuzzlePos());

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
			if (Player)
			{
				CueParams.Location = Player->CalPlayerLocalCameraStartPos() + Player->GetCameraForward() * Dist; //����� �ӽð� ��Ʈ����Ʈ �¿��� ������ ����
			}
			else
			{
				IQLAIAttackInterface* AI = Cast<IQLAIAttackInterface>(Character->GetController());
				if (AI)
				{
					AI->SetTargetPos();
					CueParams.Location = AI->GetTargetPos();
				}
			}
			//���� ASC�� �����ͼ� ExecuteGameplayCue ���� 
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
	AQLCharacterBase* Character = Cast<AQLCharacterBase>(GetActorInfo().AvatarActor.Get());

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
