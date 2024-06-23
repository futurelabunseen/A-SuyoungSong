// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QLCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameplayTag/GamplayTags.h"
#include "Physics/QLCollision.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemComponent.h"
#include "Item/QLWeaponComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/WidgetComponent.h"
#include "UI/QLNicknameWidget.h"
#include "QuadLand.h"
AQLCharacterBase::AQLCharacterBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), CurrentAttackType(ECharacterAttackType::GunAttack)
{
	//Pawn 
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	//Movement
	GetCharacterMovement()->MaxWalkSpeed = 450.f; 
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f; //조이스틱과 유사한 기기를 사용해서 미세컨트롤 가능하며, 걷기 속도를 의미함.
	GetCharacterMovement()->JumpZVelocity = 600.f; //점프
	GetCharacterMovement()->RotationRate = FRotator(0.f, 120.0f, 0.f); //FORCEINLINE TRotator(T InPitch, T InYaw, T InRoll);
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->bOrientRotationToMovement = false; //캐릭터나 액터가 이동할 때 그 방향으로 회전을 수행(마우스에 따라서 회전을 수행하도록 도와줌)
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f; //점차 멈추는 효과 
	GetCharacterMovement()->bWantsToCrouch = true;

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

	//Mesh Component
	Weapon = CreateDefaultSubobject<UQLWeaponComponent>(TEXT("Weapon"));
	Weapon->Weapon->SetupAttachment(GetMesh(), TEXT("Gun"));

	//닉네임 생성
	NicknameComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Nickname"));
	NicknameComponent->SetupAttachment(GetMesh(), TEXT("head"));
	NicknameComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("head"));

	FVector RelativeLocation(40.f, 0.f, 0.f);
	FRotator RelativeRotation(0.f, -90.f, 0.f);

	NicknameComponent->SetRelativeLocation(RelativeLocation);
	NicknameComponent->SetRelativeRotation(RelativeRotation);

	static ConstructorHelpers::FClassFinder<UQLNicknameWidget> NicknameRef(TEXT("/Game/QuadLand/UI/WBQL_Nickname.WBQL_Nickname_C"));

	if (NicknameRef.Class)
	{
		NicknameComponent->SetWidgetSpace(EWidgetSpace::World); //2D변경
		NicknameComponent->SetDrawSize(FVector2D(250.0f, 30.0f));
		NicknameComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		NicknameComponent->SetWidgetClass(NicknameRef.Class);
	}
}

void AQLCharacterBase::BeginPlay()
{
	Super::BeginPlay();

}

bool AQLCharacterBase::bIsUsingGun()
{
	return ( CurrentAttackType == ECharacterAttackType::AutomaticGunAttack || CurrentAttackType == ECharacterAttackType::GunAttack );
}

UAbilitySystemComponent* AQLCharacterBase::GetAbilitySystemComponent() const
{
	return ASC;
}

void AQLCharacterBase::SetNickname(const FString& Nickname)
{
	UQLNicknameWidget* Panel = Cast<UQLNicknameWidget>(NicknameComponent->GetWidget());

	if (Panel)
	{
		Panel->SetNickname(Nickname);
	}
}

void AQLCharacterBase::MulticastRPCInitNickname_Implementation(const FString &Nickname)
{
	SetNickname(Nickname);
}

FVector AQLCharacterBase::GetWeaponMuzzlePos()
{
	return Weapon->GetMuzzlePos();
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

void AQLCharacterBase::ServerRPCDead_Implementation()
{
	MulticastRPCDead();
}

void AQLCharacterBase::MulticastRPCDead_Implementation()
{
	SetIsDead(true);
	QL_LOG(QLLog, Log, TEXT("Current Dead? %s %d"), *GetName(), bIsDead);
}

void AQLCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	RotateBornSetting(DeltaSeconds);
	RotateNicknameSetting();
}

void AQLCharacterBase::RotateNicknameSetting()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController || !PlayerController->PlayerCameraManager)
	{
		return;
	}

	FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation(); //현재 자신의 카메라의 위치 
	FVector ActorLocation = GetActorLocation(); // 액터의 이동 위치

	FRotator LookAtRotation = (CameraLocation - ActorLocation).Rotation(); //둘이 바라보는 위치를 빼면, Rotate 값이 나오고, 위젯 자체가 회전하는 중임.
	LookAtRotation.Pitch = 0;  // Keep the widget upright

	NicknameComponent->SetWorldRotation(LookAtRotation);
}

void AQLCharacterBase::RotateBornSetting(float DeltaTime)
{
	if (bUseControllerRotationYaw == false) return; //죽으면 Yaw 동작을 껐기 때문에 해당 함수 실행 안되도록 수행한다.

	//플레이어가 살아있으면 동작한다. -> 즉, 플레이어가 죽었으면 Yaw 동작을 끔
	float Speed = CalculateSpeed();
	bool IsFalling = GetCharacterMovement()->IsFalling();

	if (bIsAiming == false)
	{
		if (Speed == 0.f && !IsFalling)
		{
			//CurrentYaw 계산
			FRotator CurrentRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
			FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentRotation, PreviousRotation);
			CurrentYaw = DeltaAimRotation.Yaw;
			if (TurningInPlace == ETurningPlaceType::ETIP_NotTurning)
			{
				InterpYaw = CurrentYaw;
			}
			bUseControllerRotationYaw = true;
			TurnInPlace(DeltaTime);
		}

		if (Speed > 0.0f || IsFalling)
		{
			PreviousRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
			CurrentYaw = 0.f;
			bUseControllerRotationYaw = true;
			TurningInPlace = ETurningPlaceType::ETIP_NotTurning;
		}
	}

	CurrentPitch = GetBaseAimRotation().Pitch;
	
	if (CurrentPitch > 90.f && !IsLocallyControlled())
	{
		FVector2D InRange(270.0f, 360.f);
		FVector2D OutRange(-90.0f, 0.f);

		CurrentPitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, CurrentPitch);
	}
}

void AQLCharacterBase::TurnInPlace(float DeltaTime)
{
	//현재 Yaw>90.0f ->오른쪽
	//방향 외적 (+/-)

	if (CurrentYaw > 90.0f)
	{
		TurningInPlace = ETurningPlaceType::ETIP_Right;
	}
	else if (CurrentYaw < -90.0f)
	{
		TurningInPlace = ETurningPlaceType::ETIP_Left;
	}
	//Aim이 꺼져있을 때만 회전..!

   //Yaw<-90.0f ->왼쪽
	if (TurningInPlace != ETurningPlaceType::ETIP_NotTurning)
	{
		InterpYaw = FMath::FInterpTo(InterpYaw, 0.f, DeltaTime, 4.f); //도는 각도를 보간하고 있구나?
		CurrentYaw = InterpYaw;

		if (FMath::Abs(CurrentYaw) < 15.0f) //어느정도 적당히 돌았음을 확인
		{
			TurningInPlace = ETurningPlaceType::ETIP_NotTurning;
			PreviousRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f); //Turn을 재조정
		}
	}

}
float AQLCharacterBase::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0; //Z는 점프축
	return Velocity.Size2D();
}


void AQLCharacterBase::ServerRPCReload_Implementation()
{	
	//MaxAmmo 없으면 실행 불가능 
	bIsReload = !bIsReload;
}
void AQLCharacterBase::ServerRPCShooting_Implementation()
{
	//만약 Stat없으면 리턴 시켜
	bIsShooting = !bIsShooting;
}


void AQLCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AQLCharacterBase, CurrentAttackType);
	DOREPLIFETIME(AQLCharacterBase, bIsShooting);
	DOREPLIFETIME(AQLCharacterBase, bIsReload); 
	DOREPLIFETIME(AQLCharacterBase, bIsAiming);
	DOREPLIFETIME(AQLCharacterBase, bIsDead);

}


void AQLCharacterBase::MulticastRPCGunAttackAnimMont_Implementation()
{
	this->PlayAnimMontage(GetAnimMontage(), 1.0f);	
}