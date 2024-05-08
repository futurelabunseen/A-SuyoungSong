// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_Dead.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"

#include "QuadLand.h"
#include "Player/QLPlayerController.h"
#include "GameplayTag/GamplayTags.h"

UQLGA_Dead::UQLGA_Dead() : Time(10.0f)
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

	AQLPlayerController* PC = Cast<AQLPlayerController>(Character->GetController());

	if (PC)
	{
		PC->OnDeathCheckDelegate.BindUObject(this, &UQLGA_Dead::OnCompleted);
		PC->ActivateDeathTimer(Time);
	}
	//�÷��̾� ������Ʈ�� �����ؾ��ҵ�! (������ ���߿� �غ���)
}

void UQLGA_Dead::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_Dead::OnCompleted()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;

	ACharacter* Character = Cast<ACharacter>(GetActorInfo().AvatarActor.Get());

	Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	Character->SetActorEnableCollision(false);
	Character->bUseControllerRotationYaw = false;
	Character->SetLifeSpan(3.0f);

	UAbilitySystemComponent *ASC = GetAbilitySystemComponentFromActorInfo();
	
	if (ASC)
	{
		ASC->AddLooseGameplayTag(CHARACTER_STATE_DEAD);
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

/*
Dead -> DELEGATE �� ����ؼ� QLPlayerController ����
QLPlayerController -> Widget ����
Widget ������, �ش� Widget Owning -> Bind

QLDeathTimerWidget ���� 



*/