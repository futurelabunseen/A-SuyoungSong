// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QLCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameplayTag/GamplayTags.h"
#include "Physics/QLCollision.h"

AQLCharacterBase::AQLCharacterBase() : CurrentAttackType(ECharacterAttackType::GunAttack)
{
	bHasGun = true;
	//Pawn 
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//Movement
	GetCharacterMovement()->MaxWalkSpeed = 450.f; 
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f; //조이스틱과 유사한 기기를 사용해서 미세컨트롤 가능하며, 걷기 속도를 의미함.
	GetCharacterMovement()->JumpZVelocity = 600.f; //점프
	GetCharacterMovement()->RotationRate = FRotator(0.f, 120.0f, 0.f); //FORCEINLINE TRotator(T InPitch, T InYaw, T InRoll);
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->bOrientRotationToMovement = true; //캐릭터나 액터가 이동할 때 그 방향으로 회전을 수행(마우스에 따라서 회전을 수행하도록 도와줌)
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f; //점차 멈추는 효과 

	//CapsuleComponent
	GetCapsuleComponent()->InitCapsuleSize(35.f, 90.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_QLCAPSULE); //나중에 변경 예정 -> object가 추가 되고 등등..

	//SkeletalMesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f,0.f,-100.f), FRotator(0.0f,-90.f,0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/QuadLand/Characters/SK_UE5_Adventures_woman_Full_01.SK_UE5_Adventures_woman_Full_01'"));

	if (SkeletalMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(SkeletalMeshRef.Object);
	}

	//AnimInstance
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceRef(TEXT("/Game/QuadLand/Animations/Blueprint/ABQL_Character.ABQL_Character_C"));

	if (AnimInstanceRef.Class)
	{
		GetMesh()->SetAnimClass(AnimInstanceRef.Class);
	}

	// Weapon Component
	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("Gun"));
}

FGameplayTag AQLCharacterBase::GetCurrentAttackTag() const
{
	if (CurrentAttackType == ECharacterAttackType::GunAttack)
	{
		return CHARACTER_EQUIP_GUNTYPEA;
	}
	else
	{
		return CHARACTER_EQUIP_NON;
	}
}
