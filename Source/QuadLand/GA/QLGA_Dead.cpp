// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_Dead.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "QuadLand.h"
#include "Character/QLCharacterBase.h"
#include "Player/QLPlayerController.h"
#include "GameplayTag/GamplayTags.h"
#include "Character/QLSpectatorPawn.h"
#include "Player/QLPlayerState.h"
#include "Game/QLGameMode.h"

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

	

	AQLCharacterBase* Character = Cast<AQLCharacterBase>(GetActorInfo().AvatarActor.Get());

	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();

	if (ASC && ASC->HasMatchingGameplayTag(CHARACTER_ATTACK_TAKENDAMAGE))
	{
		UAbilityTask_WaitGameplayEvent* WaitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CHARACTER_ATTACK_TAKENDAMAGE, nullptr, false,false);
		WaitEvent->EventReceived.AddDynamic(this,&UQLGA_Dead::SetDeadAnim);
		WaitEvent->ReadyForActivation();
	}
	else
	{
		Character->SetIsDead(true);
	}

	Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	Character->SetActorEnableCollision(false);
	Character->bUseControllerRotationYaw = false;

	Character->SetLifeSpan(10.0f);

	FTimerHandle SpawnTimerHandle;

	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &UQLGA_Dead::OnCompleted, 5.0f, false);
}

void UQLGA_Dead::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_Dead::SetDeadAnim(FGameplayEventData Payload)
{
	AQLCharacterBase* Character = Cast<AQLCharacterBase>(GetActorInfo().AvatarActor.Get());

	if (Character)
	{
		Character->SetIsDead(true);
	}
}


void UQLGA_Dead::OnCompleted()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;

	// �� �����Ƽ�� ����� ��, ���� PlayerState �� ������ ���ٸ�, ���ο� Pawn Spawn
	
	ACharacter* Character = Cast<ACharacter>(GetActorInfo().AvatarActor.Get());
	AQLPlayerState* PS = Cast<AQLPlayerState>(GetActorInfo().OwnerActor.Get());

	if (PS)
	{
		AQLPlayerController* PC = Cast<AQLPlayerController>(Character->GetController());
		FGameplayTagContainer TagContainer(CHARACTER_STATE_DANGER);

		UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
		if (PS->GetHasLifeStone() || ASC->HasAnyMatchingGameplayTags(TagContainer)) //������ �ִٸ� ������
		{
			if (IsLocallyControlled())
			{
				PC->CloseAllUI(); //UI�� ���ÿ��� ����.
			}

			if (PC && HasAuthority(&CurrentActivationInfo))
			{
				FTransform Transform = Character->GetActorTransform();
				FActorSpawnParameters Params;
				AQLSpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<AQLSpectatorPawn>(SpectatorPawnClass, Transform, Params);

				PC->Possess(Cast<APawn>(SpectatorPawn));
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
			}
		}
		else
		{
			FTimerHandle RespawnTimer;
			GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &UQLGA_Dead::RespawnTimeFunc, 3.f, false);
		}
	}
}

void UQLGA_Dead::RespawnTimeFunc()
{

	AQLPlayerState* PS = Cast<AQLPlayerState>(GetActorInfo().OwnerActor.Get());
	AQLPlayerController* PC = Cast<AQLPlayerController>(PS->GetOwner());
	
	//�˻��Ѵ�. ��� �÷��̾ � ������ �Ծ������� ���� Ȯ��.
	//���� �ƹ��� �� ������ ���� �ʾҴٸ�, respawn ����
	//������ �����ʴٸ� ���� Pawn ����.
	if (PS)
	{
		AQLGameMode* GameMode = Cast<AQLGameMode>(GetWorld()->GetAuthGameMode());

		if (GameMode)
		{
			GameMode->SpawnPlayerPawn(PC, PS->GetGenderType());
		}
	}
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