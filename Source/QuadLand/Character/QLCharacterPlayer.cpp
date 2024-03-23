// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QLCharacterPlayer.h"
#include "GameFramework/SpringArmComponent.h" //springArm - GameFramework
#include "Camera/CameraComponent.h" //camera - Camera
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "QuadLand.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AttackActionData/QLPunchAttackData.h"

AQLCharacterPlayer::AQLCharacterPlayer() : bIsFirstRunSpeedSetting(false), bHasGun(0), bHasNextPunchAttackCombo(0), CurrentCombo(0), CurrentAttackType(ECharacterAttackType::HookAttack)
{
	//springArm에 Camera를 매달을 예정
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraSpringArm->TargetArmLength = 450.0f;
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->bUsePawnControlRotation = true; //Pawn이동에 따라서 회전 예정

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; //spring Arm에 따라서 조절될 예정

	//EnhancedInput 연결
	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Move.IA_Move'"));

	if (MoveActionRef.Object)
	{
		MoveAction = MoveActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Look.IA_Look'"));

	if (LookActionRef.Object)
	{
		LookAction = LookActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> AttackActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Attack.IA_Attack'"));

	if (AttackActionRef.Object)
	{
		AttackAction= AttackActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> RunActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Run.IA_Run'"));

	if (RunActionRef.Object)
	{
		RunAction = RunActionRef.Object;
	}

	//InputContext Mapping
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputContextMappingRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/QuadLand/Inputs/IMC_Shoulder.IMC_Shoulder'"));

	if (InputContextMappingRef.Object)
	{
		InputMappingContext = InputContextMappingRef.Object;
	}
}
void AQLCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}
	SetCharacterControl();

}

void AQLCharacterPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::Look);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::Attack);

	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::RunInputPressed);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AQLCharacterPlayer::RunInputReleased);
}

void AQLCharacterPlayer::SetCharacterControl()
{

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			SubSystem->ClearAllMappings(); //모든 매핑 취소
			UInputMappingContext* NewMappingContext = InputMappingContext;

			if (NewMappingContext)
			{
				SubSystem->AddMappingContext(NewMappingContext, 0);
			}
		}
	}
}

void AQLCharacterPlayer::Move(const FInputActionValue& Value)
{
	//이동 벡터
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation(); 
	const FRotator YawRotation(0, Rotation.Yaw, 0); //회전 방향인 Yaw만 냅둔다
	//앞 방향 - x축
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	//옆 방향 - y축
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void AQLCharacterPlayer::Look(const FInputActionValue& Value)
{
	//마우스 시선 이동 처리
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AQLCharacterPlayer::Attack()
{
	QL_LOG(QLNetLog, Log, TEXT("left mouse button clicks"));

	if (bHasGun)
	{
		CurrentAttackType = ECharacterAttackType::GunAttack;
	}
	else
	{
		CurrentAttackType = ECharacterAttackType::HookAttack;
	}

	DefaultAttack();
}

void AQLCharacterPlayer::DefaultAttack()
{

	if (bHasGun)
	{
		Super::DefaultAttack(); //Default 총 공격 애니메이션 
	}
	else
	{
		QL_LOG(QLNetLog, Log, TEXT("Hook Attack ,left mouse button clicks"));

		if (CurrentCombo == 0)
		{
			PunchAttackComboBegin();
			return;
		}

		if (PunchAttackComboTimer.IsValid())
		{
			bHasNextPunchAttackCombo = true;
			//유효시간 확인 후 CurrentCombo +1 해준다.
		}
		else
		{
			bHasNextPunchAttackCombo = false;
		}
	}
}
void AQLCharacterPlayer::PunchAttackComboBegin()
{
	CurrentCombo = 1;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (IsValid(AnimInstance))
	{
		//여기서 연결
		const float AttackSpeedRate = PunchAttackData->AttackSpeedRate;
		AnimInstance->Montage_Play(AttackAnimMontage[CurrentAttackType], AttackSpeedRate);

		FOnMontageEnded MontageEnded;

		MontageEnded.BindUObject(this, &AQLCharacterPlayer::PunchAttackComboEnd);
		AnimInstance->Montage_SetEndDelegate(MontageEnded, AttackAnimMontage[CurrentAttackType]);
		PunchAttackComboTimer.Invalidate();
		SetPunchComboCheckTimer();
	}

}

void AQLCharacterPlayer::PunchAttackComboEnd(UAnimMontage* AnimMontage, bool IsProperlyEnded)
{
	ensure(CurrentCombo != 0); //0면 에러 
	CurrentCombo = 0;
}

void AQLCharacterPlayer::SetPunchComboCheckTimer()
{
	//현재 CurrentCombo 값이 몇인지 확인
	int32 ComboIndex = CurrentCombo - 1; //실질적 인덱스는 하나 작음
	ensure(PunchAttackData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	const float AttackSpeedRate = PunchAttackData->AttackSpeedRate;
	float ComboEffectiveTime = (PunchAttackData->EffectiveFrameCount[ComboIndex] / PunchAttackData->FrameRate) / AttackSpeedRate;

	QL_LOG(QLNetLog, Log, TEXT("current effective time : %f"),ComboEffectiveTime);

	if (ComboEffectiveTime > 0.0f)
	{
		//다음 액션으로 수행
		GetWorld()->GetTimerManager().SetTimer(PunchAttackComboTimer, this,
			&AQLCharacterPlayer::PunchAttackComboCheck, ComboEffectiveTime, false);
	}

}

void AQLCharacterPlayer::PunchAttackComboCheck()
{
	PunchAttackComboTimer.Invalidate(); 

	if (bHasNextPunchAttackCombo)
	{
		CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, PunchAttackData->MaxFrameCount); //다음으로 이동

		FName NextPunchSection = *FString::Printf(TEXT("%s%d"), *PunchAttackData->MontageSectionNamePrefix, CurrentCombo);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_JumpToSection(NextPunchSection);
		SetPunchComboCheckTimer(); //두개밖에 없지만 다음 기획을 위해서 일단 걸어둠.
		bHasNextPunchAttackCombo = false;
	}

}

void AQLCharacterPlayer::FarmingItem()
{
	bHasGun = true;
}

void AQLCharacterPlayer::RunInputPressed()
{

	if (bIsFirstRunSpeedSetting == false)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
		bIsFirstRunSpeedSetting = true;
	}

}

void AQLCharacterPlayer::RunInputReleased()
{
	bIsFirstRunSpeedSetting = false;
	GetCharacterMovement()->MaxWalkSpeed = 450.f;
}
