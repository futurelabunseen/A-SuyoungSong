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
	나중에 정리용도
	- 플레이어가 이동하지 못하도록 Move를 막는다.
	- 캡슐 컴포넌트도 비활성화시킨다.
	- 사라지는 Effect를 표현한다 (SetLifeSpan 사용해서 없앨 수 있음 -> 몇 초 뒤로 설정해놓고 어떻게 죽일지는 표현해보자~)
	- Inventory는 플레이어가 가지고 있음 ( 플레이어는 죽을 때 인베토리를 어떻게할까? 고민해보는게 좋을듯)
	*/

	
	APawn *DefaultCharacter = Cast<APawn>(GetActorInfo().AvatarActor.Get());

	if (DefaultCharacter)
	{
		AQLCharacterBase* Character = Cast<AQLCharacterBase>(GetActorInfo().AvatarActor.Get());
		
		if (Character->GetIsShooting())
		{
			Character->SetIsShooting(false);
		}

		if (Character)
		{

			UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();

			if (ASC && ASC->HasMatchingGameplayTag(CHARACTER_ATTACK_TAKENDAMAGE))
			{
				UAbilityTask_WaitGameplayEvent* WaitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CHARACTER_ATTACK_TAKENDAMAGE, nullptr, false, false);
				WaitEvent->EventReceived.AddDynamic(this, &UQLGA_Dead::SetDeadAnim);
				WaitEvent->ReadyForActivation();
			}
			else
			{
				Character->SetIsDead(true);
			}

			Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

			//if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_DANGER) == false)
			{
				FTimerHandle SpawnTimerHandle;

				GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &UQLGA_Dead::OnCompleted, 5.0f, false);
			}
		}

		DefaultCharacter->SetActorEnableCollision(false);
		DefaultCharacter->bUseControllerRotationYaw = false;
		DefaultCharacter->SetLifeSpan(10.0f);

		//AI는 그냥 종료된다.
		AQLPlayerState* PS = Cast<AQLPlayerState>(GetActorInfo().OwnerActor.Get());
		
		if (PS == nullptr)
		{
			bool bReplicateEndAbility = true;
			bool bWasCancelled = true;

			EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		}
	}
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

	// 이 어빌리티가 실행될 때, 만약 PlayerState 가 보석이 없다면, 새로운 Pawn Spawn
	
	ACharacter* Character = Cast<ACharacter>(GetActorInfo().AvatarActor.Get());
	AQLPlayerState* PS = Cast<AQLPlayerState>(GetActorInfo().OwnerActor.Get());

	if (PS)
	{

		QL_GASLOG(QLLog, Log, TEXT("11111111"));
		AQLPlayerController* PC = Cast<AQLPlayerController>(Character->GetController());
		FGameplayTagContainer TagContainer(CHARACTER_STATE_DANGER);

		UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
		if (PS->GetHasLifeStone() || ASC->HasMatchingGameplayTag(CHARACTER_STATE_DANGER)) //가지고 있다면 죽은것
		{
			QL_GASLOG(QLLog, Log, TEXT("222222222"));

			if (PC && HasAuthority(&CurrentActivationInfo))
			{
				QL_GASLOG(QLLog, Log, TEXT("3333333"));

				FTransform Transform = Character->GetActorTransform();
				FActorSpawnParameters Params;
				AQLSpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<AQLSpectatorPawn>(SpectatorPawnClass, Transform, Params);
				AQLGameMode* GameMode = Cast<AQLGameMode>(GetWorld()->GetAuthGameMode());

				if (GameMode)
				{
					GameMode->Dead(FName(PS->GetName()));
				}

				PC->Possess(Cast<APawn>(SpectatorPawn));
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
			}
		}
		else
		{
			QL_GASLOG(QLLog, Log, TEXT("444444"));

			FTimerHandle RespawnTimer;
			GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &UQLGA_Dead::RespawnTimeFunc, 3.f, false);
		}
	}
}

void UQLGA_Dead::RespawnTimeFunc()
{

	AQLPlayerState* PS = Cast<AQLPlayerState>(GetActorInfo().OwnerActor.Get());
	AQLPlayerController* PC = Cast<AQLPlayerController>(PS->GetOwner());
	
	//검사한다. 모든 플레이어가 어떤 보석을 먹었는지에 대해 확인.
	//만약 아무도 내 보석을 먹지 않았다면, respawn 가능
	//가지고 있지않다면 새로 Pawn 생성.

	if (PS)
	{
		ACharacter* Character = Cast<ACharacter>(GetActorInfo().AvatarActor.Get());
		AQLGameMode* GameMode = Cast<AQLGameMode>(GetWorld()->GetAuthGameMode());

		UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
		if (ASC && ASC->HasMatchingGameplayTag(CHARACTER_STATE_DANGER))
		{
			bool bReplicateEndAbility = true;
			bool bWasCancelled = false;
			FTransform Transform = Character->GetActorTransform();
			FActorSpawnParameters Params;
			QL_GASLOG(QLLog, Log, TEXT("555555555"));
			if (GameMode)
			{
				QL_GASLOG(QLLog, Log, TEXT("666666666"));
				AQLSpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<AQLSpectatorPawn>(SpectatorPawnClass, Transform, Params);
				GameMode->Dead(FName(PS->GetName()));
				PC->Possess(Cast<APawn>(SpectatorPawn));
			}
			
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
			return;
		}

		if (GameMode)
		{
			QL_GASLOG(QLLog, Log, TEXT("7777777777"));
			GameMode->SpawnPlayerPawn(PC, PS->GetGenderType());
		}
	}
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

/*
Dead -> DELEGATE 를 사용해서 QLPlayerController 연결
QLPlayerController -> Widget 연결
Widget 끝나면, 해당 Widget Owning -> Bind

QLDeathTimerWidget 연결 
*/