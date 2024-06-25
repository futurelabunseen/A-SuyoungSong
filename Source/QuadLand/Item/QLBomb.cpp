// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/QLBomb.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Physics/QLCollision.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/GamplayTags.h"
#include "Character/QLCharacterBase.h"
#include "Character/QLCharacterPlayer.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "Engine/EngineTypes.h"
#include "Item/QLWeaponComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Item/QLFirewall.h"
#include "EngineUtils.h"
#include "QuadLand.h"

// Sets default values
AQLBomb::AQLBomb()
{
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));

	RootComponent = Collision;
	Collision->SetCollisionProfileName(CPROFILE_QLPHYSICS);
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Mesh->SetupAttachment(Collision);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/QuadLand/Item3D/Bomb/Pipe_Bomb/Pipe_Bomb.Pipe_Bomb'"));
	if (MeshRef.Object)
	{
		Mesh->SetSkeletalMesh(MeshRef.Object);
	}
	Mesh->SetCollisionProfileName(CPROFILE_QLPHYSICS);
	//초기값은 BombThrower 설정
	OnActorHit.AddDynamic(this, &AQLBomb::OnActorOverlap);

	bReplicates = true;
	SetReplicateMovement(true);
}


void AQLBomb::ThrowBomb(FVector Velocity)
{
	Collision->SetSimulatePhysics(true);
	Collision->SetPhysicsLinearVelocity(Velocity);
}

void AQLBomb::PlayGameplayCue(const AQLCharacterBase* Character)
{
	UAbilitySystemComponent* SourceASC = Character->GetAbilitySystemComponent();
	if (SourceASC)
	{
		FGameplayCueParameters CueParam;

		CueParam.Location = this->GetActorLocation();

		SpawnFire(Character);
		//현재 ASC를 가져와서 ExecuteGameplayCue 실행 
		SourceASC->ExecuteGameplayCue(GAMEPLAYCUE_EFFECT_FIREWALL, CueParam);
	}
}

void AQLBomb::ExplodesBomb(AQLCharacterBase* Character)
{

	FCollisionQueryParams Params(SCENE_QUERY_STAT(DetectionPlayer), false);
	UAbilitySystemComponent* SourceASC = Character->GetAbilitySystemComponent();
	const UQLAS_WeaponStat* WeaponStat = SourceASC->GetSet<UQLAS_WeaponStat>();
	

	TArray<FHitResult> NearbyPlayers;

	bool bResult = GetWorld()->SweepMultiByChannel(
		NearbyPlayers,
		this->GetActorLocation(),
		this->GetActorLocation(),
		FQuat::Identity,
		CCHANNEL_QLACTION,
		FCollisionShape::MakeSphere(WeaponStat->GetAttackDistance()),
		Params
	);

	if (bResult)
	{

		for (const auto& Player : NearbyPlayers)
		{
			AQLCharacterBase* TmpPawn = Cast<AQLCharacterBase>(Player.GetActor());
			//AQLCharacterBase를 상속받은 Player와 AI에게 Effect 적용
			if (TmpPawn)
			{
				FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
				FGameplayEffectSpecHandle EffectSpecHandle = SourceASC->MakeOutgoingSpec(AttackDamageEffect, 1, EffectContextHandle);
				
				if (EffectSpecHandle.IsValid())
				{
					EffectContextHandle.AddOrigin(this->GetActorLocation()); //Bomb 부딪힌 위치를 전달
					EffectContextHandle.AddSourceObject(WeaponStat); //현재 Bomb을 사용한 캐릭터의 스탯 정보 전달
					//이 스탯은 데미지 전달을 위함.

					UAbilitySystemComponent* TargetASC = TmpPawn->GetAbilitySystemComponent();
					//부딪힌 모든 Pawn의 ASC를 가져와서 Effect를 실행시킨다.
					if (TargetASC)
					{
						EffectSpecHandle.Data->SetContext(EffectContextHandle);
						TargetASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
					}
				}
			}
		}
	}

	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(Character);
	if (Player)
	{
		QL_LOG(QLLog, Log, TEXT("Current Bomb 해제"));
		Player->ClientRPCThrowBomb();
	}
	PlayGameplayCue(Character);
}

void AQLBomb::SpawnFire(const AQLCharacterBase* Character)
{

	if (HasAuthority())
	{
		FCollisionQueryParams Params(SCENE_QUERY_STAT(FireCollision), false);
		UAbilitySystemComponent* SourceASC = Character->GetAbilitySystemComponent();
		const UQLAS_WeaponStat* WeaponStat = SourceASC->GetSet<UQLAS_WeaponStat>();

		TArray<FHitResult> NearbyActors;
		//ItemDetectionSocket
		bool bResult = GetWorld()->SweepMultiByChannel(
			NearbyActors,
			this->GetActorLocation(),
			this->GetActorLocation(),
			FQuat::Identity,
			CCHANNEL_QLFIRE,
			FCollisionShape::MakeSphere(WeaponStat->GetAttackDistance()),
			Params
		);

		if (bResult)
		{

			for (const auto& NearbyActor : NearbyActors)
			{
				//Actor라면.. 움직일 수 있음..
				AQLCharacterBase* TmpPawn = Cast<AQLCharacterBase>(NearbyActor.GetActor());
				FActorSpawnParameters SpawnParams;
				FVector Location = NearbyActor.GetActor()->GetActorLocation();

				if (TmpPawn)
				{
					AQLFirewall* FireActor = GetWorld()->SpawnActor<AQLFirewall>(Fire, Location, FRotator::ZeroRotator, SpawnParams);
					FireActor->AttachToActor(TmpPawn, FAttachmentTransformRules::KeepWorldTransform);
				}
				else
				{
					GetWorld()->SpawnActor<AQLFirewall>(Fire, Location, FRotator::ZeroRotator, SpawnParams);
				}
			}
		}
	}

	SetLifeSpan(0.5f);
}

void AQLBomb::OnActorOverlap(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	AQLCharacterBase* Character = Cast< AQLCharacterBase>(GetOwner());

	if (Character)
	{
		ExplodesBomb(Character);
	}
	Collision->SetSimulatePhysics(false);
	Collision->SetWorldLocation(Mesh->GetComponentLocation());
}