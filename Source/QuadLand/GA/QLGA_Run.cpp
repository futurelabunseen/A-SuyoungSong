// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_Run.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/GamplayTags.h"
#include "Character/QLCharacterMovementComponent.h"


UQLGA_Run::UQLGA_Run()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UQLGA_Run::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
}

void UQLGA_Run::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	StopStamina();
}

void UQLGA_Run::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	UQLCharacterMovementComponent* QLMovement = Cast< UQLCharacterMovementComponent>(Character->GetMovementComponent());
	if (QLMovement)
	{
		QLMovement->SetSprintCommand();
		//ReduceStamina();
		GetWorld()->GetTimerManager().SetTimer(StaminaTimer,this, &UQLGA_Run::ReduceStamina, 1.0f, true, 0.0f);//타이머를 걸어서 1초마다 실행 -> 만약 Released에 의해서 bIsRunning 멈췄으면 Handle 해제
	}

}

void UQLGA_Run::ReduceStamina()
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();

	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(ReduceStaminaEffect);

	if (EffectSpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle);
	}
}

void UQLGA_Run::StopStamina()
{
	if (StaminaTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(StaminaTimer);
		StaminaTimer.Invalidate();
	}
	ACharacter* Character = Cast<ACharacter>(GetActorInfo().AvatarActor.Get());
	UQLCharacterMovementComponent* QLMovement = Cast< UQLCharacterMovementComponent>(Character->GetMovementComponent());
	if (QLMovement)
	{
		QLMovement->UnSetSprintCommand();
	}
	OnCompleted();
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