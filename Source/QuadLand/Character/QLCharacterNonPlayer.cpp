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
#include "Perception/AISenseConfig_Sight.h"
#include "Game/QLGameMode.h"
#include "QuadLand.h"
#include "GameData/QLNickname.h"
#include "Math/NumericLimits.h"

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

	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	AIPerception->ConfigureSense(*SightConfig);

	Weapon->Weapon->SetSkeletalMesh(GunMesh);
	
//	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AQLCharacterNonPlayer::UpdateTargetPerception);
	AIPerception->OnPerceptionUpdated.AddDynamic(this, &AQLCharacterNonPlayer::PerceptionUpdated);
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

	if (!DefaultEffects) //Gameplay Effect를 통해서 모든 어트리뷰트 기본값으로 초기화
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

	FGameplayEffectSpecHandle NewHandle = ASC->MakeOutgoingSpec(DefaultEffects, 1, EffectContext);

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

	int Idx = FMath::RandRange(0, Nicknames->Nickname.Num());
	if (Nicknames->Nickname.Num() > Idx)
	{
		MulticastRPCInitNickname(Nicknames->Nickname[Idx]);
	}
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
			GameMode->DeadNonPlayer(FName(this->GetName()));
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

void AQLCharacterNonPlayer::CanSelectTarget(bool InSelectTarget)
{
	bSelectTarget = InSelectTarget;
}

void AQLCharacterNonPlayer::CheckBoxOverlap()
{
}

void AQLCharacterNonPlayer::UpdateTargetPerception(AActor* Actor, FAIStimulus Stimulus)
{
	if (bSelectTarget) return;
	AQLAIController* AIController = GetController<AQLAIController>();
	//1. 시야에 들어온다
	//2. 소리가 들리는 쪽으로 고개를 돌린다.:

	if (AIController == nullptr)
	{
		return;
	}
	UBlackboardComponent *BC = AIController->GetBlackboardComponent();

	ACharacter* Character = Cast<ACharacter>(Actor); //NonPlayer,Player 모두 찾아냄.
	if (Character == nullptr)
	{
		return;
	}

	if (BC == nullptr)
	{
		return;
	}
	
	if (Actor == this)
	{
		return;
	}

	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		QL_LOG(QLLog, Warning, TEXT("TargetActor"));
		BC->SetValueAsObject(TEXT("TargetActor"), Actor);
	}

}

void AQLCharacterNonPlayer::PerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	AQLAIController* AIController = GetController<AQLAIController>();
	//1. 시야에 들어온다
	//2. 소리가 들리는 쪽으로 고개를 돌린다.:

	if (AIController == nullptr)
	{
		return;
	}
	UBlackboardComponent* BC = AIController->GetBlackboardComponent();

	if (BC == nullptr)
	{
		return;
	}
	AActor* Target = nullptr;

	float MinDist = TNumericLimits<float>::Max();

	FVector CurrentPos = this->GetActorLocation();
	CurrentPos.Z = 0; //높이는 필요하지 않음

	for (const auto TargetActor : UpdatedActors)
	{
		if (TargetActor == this) continue;

		FVector TargetPos = TargetActor->GetActorLocation();

		TargetPos.Z = 0;

		float DistToTarget=FVector::Dist(TargetPos, CurrentPos);

		if (MinDist > DistToTarget)
		{
			MinDist = DistToTarget;
			Target = TargetActor;
		}
	}

	if (Target != nullptr)
	{
		//타겟 유지
		BC->SetValueAsObject(TEXT("TargetActor"), Target);
	}
}
