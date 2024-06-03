// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QLCharacterNonPlayer.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "Item/QLWeaponComponent.h"
#include "AI/QLAIController.h"
#include "Perception/AIPerceptionComponent.h"

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
	Weapon->Weapon->SetSkeletalMesh(GunMesh);

	AIControllerClass = AQLAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

UAbilitySystemComponent* AQLCharacterNonPlayer::GetAbilitySystemComponent()
{
	return ASC;
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
}

void AQLCharacterNonPlayer::BeginPlay()
{
	Super::BeginPlay();

}
