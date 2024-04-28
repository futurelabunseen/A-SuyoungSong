// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_Run.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/GamplayTags.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "Character/QLCharacterMovementComponent.h"
#include "QuadLand.h"

UQLGA_Run::UQLGA_Run()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

//Client�κ��� �Է��� ����
void UQLGA_Run::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	StopStamina();
	ServerRPCStop();
}

void UQLGA_Run::ServerRPCStop_Implementation()
{
	QL_GASLOG(QLNetLog, Warning, TEXT("Reduce Stamina"));
	StopStamina();
}

void UQLGA_Run::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CommitAbility(Handle, ActorInfo, ActivationInfo); //CommitAbility 
	
	ACharacter* Character = Cast<ACharacter>(GetActorInfo().AvatarActor.Get());
	UQLCharacterMovementComponent* QLMovement = Cast< UQLCharacterMovementComponent>(Character->GetMovementComponent());
	if (QLMovement)
	{
		QLMovement->SetSprintCommand();
	}

	if (!StaminaTimer.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(StaminaTimer, this, &UQLGA_Run::ReduceStamina, 0.25f, true, 0.0f);//Ÿ�̸Ӹ� �ɾ 1�ʸ��� ���� -> ���� Released�� ���ؼ� bIsRunning �������� Handle ����
	}
}

void UQLGA_Run::ReduceStamina()
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();

	FGameplayTagContainer Tag(CHARACTER_STATE_STOP);
	if (SourceASC->HasAnyMatchingGameplayTags(Tag))
	{
		StopStamina();
		return;
	}

	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(ReduceStaminaEffect);

	if (EffectSpecHandle.IsValid())
	{
		QL_GASLOG(QLNetLog, Log, TEXT("Reduce Stamina"));
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle);
	}
}

void UQLGA_Run::StopStamina()
{
	GetWorld()->GetTimerManager().ClearTimer(StaminaTimer);
	StaminaTimer.Invalidate();
	
	ACharacter* Character = Cast<ACharacter>(GetActorInfo().AvatarActor.Get());
	UQLCharacterMovementComponent* QLMovement = Cast< UQLCharacterMovementComponent>(Character->GetMovementComponent());
	if (QLMovement)
	{
		QLMovement->UnSetSprintCommand();
	}
	
	OnCompleted();
}

void UQLGA_Run::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	StopStamina();
	CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UQLGA_Run::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_Run::OnCompleted()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}