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
#include "Sound/SoundCue.h"
#include "Interface/QLAIAttackInterface.h"
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

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	const UQLAS_WeaponStat* WeaponStat = SourceASC->GetSet<UQLAS_WeaponStat>();

	if (SourceASC->HasAnyMatchingGameplayTags(CancelTag)) //이 어빌리티를 실행할 때 CancelTag에 지정된 태그는 받지 않는다.
	{
		return false;
	}

	if (WeaponStat && WeaponStat->GetCurrentAmmo() <= 0.0f) //Ammo Cnt 개수를 체크한다 만약 0이라면, 어빌리티를 실행하지 않고 종료한다.
	{
		return false;
	}
	AQLCharacterBase* Character = Cast<AQLCharacterBase>(GetActorInfo().AvatarActor.Get());
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(Character);

	if (Player && Player->GetIsJumping()) //점프할 때 총을 쏠 수 없다.
	{
		return false;
	}

	return Result;
}

void UQLGA_AttackUsingGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//총을 쏜다면 플레이어를 회전 시킨다.
	AQLCharacterBase* Character = Cast<AQLCharacterBase>(GetActorInfo().AvatarActor.Get());
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(Character);
	float AnimSpeedRate = 1.0f;


	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	const UQLAS_WeaponStat* WeaponStat = SourceASC->GetSet<UQLAS_WeaponStat>();

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

	if (IsLocallyControlled()) //로컬에 있는 카메라를 흔들어 준다.
	{
		if (Player)
		{
			APlayerCameraManager* LocalCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
			LocalCamera->StartCameraShake(CameraShakeClass);
		}
	}

	if (HasAuthority(&ActivationInfo)) //서버 내에서 AI에게 Noise를 전달한다.
	{
		Character->MakeNoise();
		MulticastRPCShoot(Character); //총 사운드
	}

	UAnimMontage* AnimMontageUsingGun = Character->GetAnimMontage();

	//애니메이션 몽타주 동작
	UAbilityTask_PlayMontageAndWait* AttackUsingGunMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("GunAnimMontage"), AnimMontageUsingGun, AnimSpeedRate);
	AttackUsingGunMontage->OnCompleted.AddDynamic(this, &UQLGA_AttackUsingGun::OnCompletedCallback);
	AttackUsingGunMontage->OnInterrupted.AddDynamic(this, &UQLGA_AttackUsingGun::OnInterruptedCallback);

	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(ReduceAmmoCntEffect); //총알 횟수를 줄여주는 이펙트 발동

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
				CueParams.Location = Player->CalPlayerLocalCameraStartPos() + Player->GetCameraForward() * Dist; //쏠 거리를 미리 지정
			}
			else
			{
				IQLAIAttackInterface* AI = Cast<IQLAIAttackInterface>(Character->GetController());
				if (AI)
				{
					AI->SetTargetPos();
					CueParams.Location = AI->GetTargetPos(); //AI는 SetTargetPos를 사용해서 범위랜덤을 고려해서 위치를 지정
				}
			}
			//현재 ASC를 가져와서 ExecuteGameplayCue 실행 
			SourceASC->ExecuteGameplayCue(GAMEPLAYCUE_EFFECT_TRACEBYGUN, CueParams);
		}
	}

	if (SourceASC)
	{
		//SourceASC->AddTag
		FGameplayTagContainer TargetTag(CHARACTER_ATTACK_HITCHECK);
		SourceASC->TryActivateAbilitiesByTag(TargetTag, false); //Attack_HITCHECK + EQUIP 
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

void UQLGA_AttackUsingGun::MulticastRPCShoot_Implementation(AQLCharacterBase* Character)
{
	UGameplayStatics::PlaySoundAtLocation(Character->GetMesh(), GunSound, Character->GetWeaponMuzzlePos());
}
