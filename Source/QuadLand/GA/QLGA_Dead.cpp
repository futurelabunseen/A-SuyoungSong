// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_Dead.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"

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

	AQLCharacterBase* Character = Cast<AQLCharacterBase>(GetActorInfo().AvatarActor.Get());
	
	if (HasAuthority(&ActivationInfo))
	{
		if (Character)
		{
			Character->SetIsDead(!Character->GetIsDead());
		}
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

void UQLGA_Dead::OnCompleted()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;

	// 이 어빌리티가 실행될 때, 만약 PlayerState 가 보석이 없다면, 새로운 Pawn Spawn
	
	ACharacter* Character = Cast<ACharacter>(GetActorInfo().AvatarActor.Get());
	AQLPlayerController* PC = Cast<AQLPlayerController>(Character->GetController());

	AQLPlayerState* PS = Cast<AQLPlayerState>(GetActorInfo().OwnerActor.Get());


	if (PS)
	{
		if (PS->GetHasLifeStone()) //가지고 있다면 죽은것
		{
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
	
	//검사한다. 모든 플레이어가 어떤 보석을 먹었는지에 대해 확인.
	//만약 아무도 내 보석을 먹지 않았다면, respawn 가능
	//가지고 있지않다면 새로 Pawn 생성.
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
Dead -> DELEGATE 를 사용해서 QLPlayerController 연결
QLPlayerController -> Widget 연결
Widget 끝나면, 해당 Widget Owning -> Bind

QLDeathTimerWidget 연결 
*/