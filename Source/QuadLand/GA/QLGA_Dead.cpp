// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_Dead.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"

#include "QuadLand.h"
#include "Player/QLPlayerController.h"
#include "GameplayTag/GamplayTags.h"

UQLGA_Dead::UQLGA_Dead() 
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UQLGA_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	/*
	���߿� �����뵵
	- �÷��̾ �̵����� ���ϵ��� Move�� ���´�.
	- ĸ�� ������Ʈ�� ��Ȱ��ȭ��Ų��.
	- ������� Effect�� ǥ���Ѵ� (SetLifeSpan ����ؼ� ���� �� ���� -> �� �� �ڷ� �����س��� ��� �������� ǥ���غ���~)
	- Inventory�� �÷��̾ ������ ���� ( �÷��̾�� ���� �� �κ��丮�� ����ұ�? ����غ��°� ������)
	*/

	ACharacter* Character = Cast<ACharacter>(GetActorInfo().AvatarActor.Get());

	Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	Character->SetActorEnableCollision(false);
	Character->bUseControllerRotationYaw = false;
	Character->SetLifeSpan(5.0f);
	AQLPlayerController* PC = Cast<AQLPlayerController>(Character->GetController());

	if (IsLocallyControlled()&&PC)
	{
		PC->Loose();
	
		//���� ī�޶� Possess
	}
	OnCompleted();
}

void UQLGA_Dead::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_Dead::OnCompleted()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

/*
Dead -> DELEGATE �� ����ؼ� QLPlayerController ����
QLPlayerController -> Widget ����
Widget ������, �ش� Widget Owning -> Bind

QLDeathTimerWidget ���� 



*/