// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_Dead.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"

#include "QuadLand.h"
#include "Character/QLCharacterPlayer.h"
#include "Player/QLPlayerController.h"
#include "GameplayTag/GamplayTags.h"
#include "Character/QLSpectatorPawn.h"

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

	UE_LOG(LogTemp, Log, TEXT("This is Dead"));
	ACharacter* Character = Cast<ACharacter>(GetActorInfo().AvatarActor.Get());

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

	ACharacter* Character = Cast<ACharacter>(GetActorInfo().AvatarActor.Get());
	AQLPlayerController* PC = Cast<AQLPlayerController>(Character->GetController());

	if (PC && HasAuthority(&CurrentActivationInfo))
	{
		FTransform Transform = Character->GetActorTransform();
		FActorSpawnParameters Params;
		AQLSpectatorPawn *SpectatorPawn =GetWorld()->SpawnActor<AQLSpectatorPawn>(SpectatorPawnClass, Transform, Params);

		PC->Possess(Cast<APawn>(SpectatorPawn));
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

/*
Dead -> DELEGATE 를 사용해서 QLPlayerController 연결
QLPlayerController -> Widget 연결
Widget 끝나면, 해당 Widget Owning -> Bind

QLDeathTimerWidget 연결 



*/