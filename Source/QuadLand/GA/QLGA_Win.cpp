// Fill out your copyright notice in the Description page of Project Settings.


#include "QLGA_Win.h"
#include "Player/QLPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/QLPlayerController.h"
#include "Character/QLCharacterPlayer.h"
#include "AbilitySystemComponent.h"
#include "QuadLand.h"
#include "Player/QLPlayerState.h"
UQLGA_Win::UQLGA_Win()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UQLGA_Win::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());
	
	if (Character)
	{
		if (Character->GetIsShooting())
		{
			Character->SetIsShooting(false);
		}

		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		Character->bUseControllerRotationYaw = false;
		Character->StopAim();
	}
	
	AQLPlayerController* PC = Cast<AQLPlayerController>(Character->GetController());
	
	if (HasAuthority(&ActivationInfo))
	{
		AQLPlayerState* PS = Cast<AQLPlayerState>(Character->GetPlayerState());
		PS->SetbIsWin(true);
	}

	if (IsLocallyControlled()&&PC)
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
