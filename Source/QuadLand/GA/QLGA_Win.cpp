// Fill out your copyright notice in the Description page of Project Settings.


#include "QLGA_Win.h"
#include "Player/QLPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Player/QLPlayerController.h"
#include "QuadLand.h"
UQLGA_Win::UQLGA_Win()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UQLGA_Win::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	ACharacter* Character = Cast<ACharacter>(GetActorInfo().AvatarActor.Get());
	Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	AQLPlayerController* PC = Cast<AQLPlayerController>(Character->GetController());

	if (PC)
	{
		PC->Win();
	}

	OnCompleted();
}

void UQLGA_Win::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_Win::OnCompleted()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);	
}
