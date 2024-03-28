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
#include "Player/QLPlayerState.h"
#include "Player/QLPlayerController.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"


AQLCharacterPlayer::AQLCharacterPlayer() : bIsFirstRunSpeedSetting(false), bHasGun(0), bHasNextPunchAttackCombo(0), CurrentCombo(0), bPressedFarmingKey(0)
{
	ASC = nullptr;
	CurrentAttackType = ECharacterAttackType::HookAttack; //default

	//springArm에 Camera를 매달을 예정
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraSpringArm->TargetArmLength = 300.0f;
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->bUsePawnControlRotation = true; //Pawn이동에 따라서 회전 예정

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; //spring Arm에 따라서 조절될 예정

	CameraSpringArm->SetRelativeLocation(FVector(0.0f,0.0f,30.0f));
	CameraSpringArm->SocketOffset = FVector(0.0f, 40.0f, 40.0f);
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

	static ConstructorHelpers::FObjectFinder<UInputAction> FarmingActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Faming.IA_Faming'"));

	if (RunActionRef.Object)
	{
		FarmingAction = FarmingActionRef.Object;
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

/// <summary>
/// PossessedBy 자체가 서버에서만 호출되기 때문에, 아래 Ability System 등록은 서버에서만 수행
/// </summary>
/// <param name="NewController"></param>
void AQLCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AQLPlayerState* QLPlayerState = Cast<AQLPlayerState>(GetPlayerState());

	if (QLPlayerState)
	{
		ASC = QLPlayerState->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(QLPlayerState, this);

		for (const auto& Ability : StartAbilities)
		{
			FGameplayAbilitySpec Spec(Ability);
			ASC->GiveAbility(Spec);
		}

		for (const auto& Ability : InputAbilities)
		{
			FGameplayAbilitySpec Spec(Ability.Value);
			Spec.InputID = Ability.Key;
			ASC->GiveAbility(Spec);
		}

		SetupGASInputComponent();

		APlayerController* PlayerController = CastChecked<APlayerController>(NewController);
		PlayerController->ConsoleCommand(TEXT("showdebug abilitysystem"));
	}
}

void AQLCharacterPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::Look);
	EnhancedInputComponent->BindAction(FarmingAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::FarmingItem);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::RunInputPressed);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AQLCharacterPlayer::RunInputReleased);

	SetupGASInputComponent();
}

void AQLCharacterPlayer::SetupGASInputComponent()
{
	if (IsValid(ASC)&&IsValid(InputComponent))
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::GASInputPressed, 0);
	}
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

UAbilitySystemComponent* AQLCharacterPlayer::GetAbilitySystemComponent() const
{
	return ASC;
}

//Client Only 
void AQLCharacterPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AQLPlayerState* QLPlayerState = GetPlayerState<AQLPlayerState>();

	if (QLPlayerState)
	{
		ASC = Cast<UAbilitySystemComponent>(QLPlayerState->GetAbilitySystemComponent());

		ASC->InitAbilityActorInfo(QLPlayerState, this);
		QL_LOG(QLNetLog, Log, TEXT("Current Class is called by Client only"));
	}
}

void AQLCharacterPlayer::PickupItem()
{
	if (bPressedFarmingKey)
	{
		//카메라의 월드 위치를 가져온다.
		//FVector StartPos = Camera->GetCameraLoc
		//카메라의 방향을 가져온다.
		//카메라 위치에서 거리 50cm로 라인트레이스를 쏜다.
		//해당 결과에 오브젝트가 있는지 여부를 확인한다.
	}
	//무조건 false로 만듬.
	bPressedFarmingKey = false;
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
}

void AQLCharacterPlayer::GASInputPressed(int32 InputID)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputID);
	
	if (Spec)
	{
		Spec->InputPressed = true; //해당키를 눌렀음을 알려줌 

		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputPressed(*Spec);
		}
		else
		{
			ASC->TryActivateAbility(Spec->Handle);
		}
	}
}

void AQLCharacterPlayer::GASInputReleased(int32 InputID)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputID);

	if (Spec)
	{
		Spec->InputPressed = false; //해당키를 눌렀음을 알려줌
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputReleased(*Spec);
		}
	}
}

void AQLCharacterPlayer::FarmingItem()
{
	bPressedFarmingKey = true;
	PickupItem();
	//Raycast 를 사용해서 해당 오브젝트 파악하기
//	FVector Start = Get
	
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

