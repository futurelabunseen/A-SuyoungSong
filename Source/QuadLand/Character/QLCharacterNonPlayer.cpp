// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QLCharacterNonPlayer.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "Item/QLWeaponComponent.h"
#include "AI/QLAIController.h"
#include "GameplayTag/GamplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Game/QLGameMode.h"
#include "QuadLand.h"

AQLCharacterNonPlayer::AQLCharacterNonPlayer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));

	WeaponStat = CreateDefaultSubobject<UQLAS_WeaponStat>(TEXT("WeaponStat"));
	PlayerStat = CreateDefaultSubobject<UQLAS_PlayerStat>(TEXT("PlayerStat"));

	bHasGun = true;
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GunMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/QuadLand/Weapons/Assault_Rifle_A.Assault_Rifle_A'"));

	if (GunMeshRef.Object)
	{
		GunMesh = GunMeshRef.Object;
	}

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	UAISenseConfig_Hearing *HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearConfig"));
	AIPerception->ConfigureSense(*HearingConfig);

	Weapon->Weapon->SetSkeletalMesh(GunMesh);

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AQLCharacterNonPlayer::UpdateTargetPerception);

	AIControllerClass = AQLAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}


void AQLCharacterNonPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!ASC)
	{
		return;
	}

	if (!DefaultAttributes) //Gameplay Effect를 통해서 모든 어트리뷰트 기본값으로 초기화
	{
		return;
	}

	ASC->InitAbilityActorInfo(this, this);

	for (const auto& Ability : StartAbilities)
	{
		FGameplayAbilitySpec Spec(Ability);
		ASC->GiveAbility(Spec);
	}

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = ASC->MakeOutgoingSpec(DefaultAttributes, 1, EffectContext);

	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), ASC.Get());
	}

	CurrentAttackType = ECharacterAttackType::GunAttack;
	ASC->AddLooseGameplayTag(CHARACTER_EQUIP_GUNTYPEA);
}

void AQLCharacterNonPlayer::BeginPlay()
{
	Super::BeginPlay();
	ASC->RegisterGameplayTagEvent(CHARACTER_ATTACK_TAKENDAMAGE, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLCharacterNonPlayer::AQLCharacterNonPlayer::AttachTakeDamageTag);
	ASC->RegisterGameplayTagEvent(CHARACTER_STATE_DEAD, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLCharacterNonPlayer::Dead);
}


FRotator AQLCharacterNonPlayer::GetBaseAimRotation() const
{
	FRotator PreRotator = Super::GetBaseAimRotation();
	AQLAIController* AIController = GetController<AQLAIController>();

	if (AIController)
	{
		const APawn* Pawn = AIController->GetTarget(); //상대방

		if (Pawn)
		{
			FVector Dir = (Pawn->GetActorLocation() - GetActorLocation()); //상대방의 Pitch값, 나.Pitch - 상대방.Pitch
			PreRotator.Pitch = Dir.Rotation().Pitch;
		}
	}
	return PreRotator;
}

void AQLCharacterNonPlayer::AttachTakeDamageTag(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount >= 1)
	{
		bTakeDamage = true;
	}
}

void AQLCharacterNonPlayer::Dead(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (bIsDead == false)
	{
		FGameplayTagContainer TargetTag(CHARACTER_STATE_DEAD);
		ASC->TryActivateAbilitiesByTag(TargetTag);

		AQLGameMode* GameMode = Cast<AQLGameMode>(GetWorld()->GetAuthGameMode());

		if (GameMode)
		{
			GameMode->DeadNonPlayer(this);
			GameMode->GetWinner(CallbackTag, NewCount);
		}
	}
	bIsDead = true;
}

bool AQLCharacterNonPlayer::CanTakeDamage()
{
	return bTakeDamage; //false 는 도망친다. 
}

void AQLCharacterNonPlayer::StopDamage()
{
	bTakeDamage = false;
}

void AQLCharacterNonPlayer::CheckBoxOverlap()
{
}

void AQLCharacterNonPlayer::UpdateTargetPerception(AActor* Actor, FAIStimulus Stimulus)
{
	AQLAIController* AIController = GetController<AQLAIController>();

	if (AIController)
	{
		UBlackboardComponent* BC = AIController->GetBlackboardComponent();

		if (BC)
		{
			BC->SetValueAsVector(TEXT("PatrolPosition"), Stimulus.StimulusLocation);
		}
	}
}