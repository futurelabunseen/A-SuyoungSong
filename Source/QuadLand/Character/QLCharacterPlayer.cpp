// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QLCharacterPlayer.h"
#include "GameFramework/SpringArmComponent.h" //springArm - GameFramework
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h" //camera - Camera
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "AbilitySystemComponent.h"
#include "EngineUtils.h"

#include "Item/QLItemBox.h"
#include "AttackActionData/QLPunchAttackData.h"
#include "Player/QLPlayerState.h"
#include "Player/QLPlayerController.h"
#include "Physics/QLCollision.h"
#include "GameData/QLWeaponStat.h"
#include "QuadLand.h"

AQLCharacterPlayer::AQLCharacterPlayer() : bIsRunning(false), bHasNextPunchAttackCombo(0), CurrentCombo(0), bPressedFarmingKey(0), FarmingTraceDist(1000.0f), MaxArmLength(300.0f)//, bIsTurning(false)
{
	bHasGun = false;
	ASC = nullptr;
	CurrentAttackType = ECharacterAttackType::HookAttack; //default

	//springArm에 Camera를 매달을 예정
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraSpringArm->TargetArmLength = MaxArmLength;
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->bUsePawnControlRotation = true; //Pawn이동에 따라서 회전 예정
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; //spring Arm에 따라서 조절될 예정

	CameraSpringArm->TargetArmLength =300.f;
	CameraSpringArm->SetRelativeLocation(FVector(0.0f,30.0f,0.0f));
	CameraSpringArm->SocketOffset = FVector(0.0f, 15.0f, 48.0f);
	
	// Weapon Component
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("Gun"));

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


	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Jump.IA_Jump'"));

	if (JumpActionRef.Object)
	{
		JumpAction = JumpActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> CrunchActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Crunch.IA_Crunch'"));

	if (CrunchActionRef.Object)
	{
		CrunchAction = CrunchActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> AimActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Aim.IA_Aim'"));

	if (AimActionRef.Object)
	{
		AimAction = AimActionRef.Object;
	}
	//InputContext Mapping
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputContextMappingRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/QuadLand/Inputs/IMC_Shoulder.IMC_Shoulder'"));

	if (InputContextMappingRef.Object)
	{
		InputMappingContext = InputContextMappingRef.Object;
	}

	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this,&AQLCharacterPlayer::EquipWeapon)));

	static ConstructorHelpers::FObjectFinder<UCurveFloat> AimCurveRef(TEXT("/Script/Engine.CurveFloat'/Game/QuadLand/Curve/AimAlpha.AimAlpha'"));

	if (AimCurveRef.Object)
	{
		AimAlphaCurve = AimCurveRef.Object;
	}

	ZoomInTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ZoominTimeline"));
	InterpFunction.BindUFunction(this, FName(TEXT("TimelineFloatReturn")));
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

	ZoomInTimeline->AddInterpFloat(AimAlphaCurve, InterpFunction, FName{ TEXT("AimAlpha") });
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

	EnhancedInputComponent->BindAction(FarmingAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::FarmingItemPressed);
	EnhancedInputComponent->BindAction(FarmingAction, ETriggerEvent::Completed, this, &AQLCharacterPlayer::FarmingItemReleased);

	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::RunInputPressed);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AQLCharacterPlayer::RunInputReleased);

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

	EnhancedInputComponent->BindAction(CrunchAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::Crunch);

	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::Aim);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AQLCharacterPlayer::StopAiming);

	SetupGASInputComponent();
}

void AQLCharacterPlayer::SetupGASInputComponent()
{
	if (IsValid(ASC)&&IsValid(InputComponent))
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::GASInputPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &AQLCharacterPlayer::GASInputReleased);
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


void AQLCharacterPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	FVector CameraLocStart = CalPlayerLocalCameraStartPos(); //카메라의 시작점 -> Spring Arm 만큼 앞으로 이동한 다음 물체가 있는지 확인

	FVector LocEnd = CameraLocStart + (GetCameraForward() * FarmingTraceDist);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ItemFarmingLineTrace), false, this); //식별자 

	FHitResult OutHitResult;

	bool bResult = GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		CameraLocStart,
		LocEnd,
		CCHANNEL_QLITEMACTION,
		Params
	);

	AQLPlayerController* PC = Cast<AQLPlayerController>(GetController());

	if (!PC)
	{
		return;
	}

	if (bResult)
	{
		//PC->SetVisibleFarming();
		if (bPressedFarmingKey) //꼭 무기라고 단정 지을 수는 없음. 
		{
			AQLItemBox* Item = Cast<AQLItemBox>(OutHitResult.GetActor());

			if (Item == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Item is not founded"));
				return;
			}
			TakeItemActions[static_cast<uint8>(Item->Stat->ItemType)].ItemDelegate.ExecuteIfBound(Item->Stat);
			OutHitResult.GetActor()->SetActorEnableCollision(false);
			OutHitResult.GetActor()->SetActorHiddenInGame(true);
		}
		bPressedFarmingKey = false;
	}
	else
	{
		//PC->SetInvisibleFarming();
	}

//#if ENABLE_DRAW_DEBUG
//	FColor Color = bResult ? FColor::Green : FColor::Red;
//	DrawDebugLine(GetWorld(), CameraLocStart, LocEnd, Color, false, 5.0f);
//#endif

}

void AQLCharacterPlayer::EquipWeapon(UQLItemData* InItemInfo)
{
	if (InItemInfo == nullptr) return;

	CurrentAttackType = ECharacterAttackType::GunAttack;
	//Weapon 위치는 소켓 
	UQLWeaponStat* WeaponStat = CastChecked<UQLWeaponStat>(InItemInfo);
	AQLPlayerState* PS = CastChecked<AQLPlayerState>(GetPlayerState());

	QL_LOG(QLLog, Log, TEXT("%d %d"), Weapon, WeaponStat->WeaponMesh);

	if (Weapon && WeaponStat->WeaponMesh)
	{
		Weapon->SetSkeletalMesh(WeaponStat->WeaponMesh);
	}


	bHasGun = true;
}

FVector AQLCharacterPlayer::CalPlayerLocalCameraStartPos()
{
	return  Camera->GetComponentLocation() + GetCameraForward() * CameraSpringArm->TargetArmLength;
}

FVector AQLCharacterPlayer::GetCameraForward()
{
	return  Camera->GetForwardVector();
}


void AQLCharacterPlayer::Move(const FInputActionValue& Value)
{
	
	////이동 벡터
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

void AQLCharacterPlayer::GASInputPressed()
{
	if(bIsRunning && CurrentAttackType == ECharacterAttackType::GunAttack)
	{
		return;
	}
	uint8 InputAttackSpecNumber = static_cast<uint8>(CurrentAttackType);

	
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputAttackSpecNumber);
	
	if (Spec)
	{
		QL_LOG(QLNetLog, Log, TEXT("GAS current ID %d"), InputAttackSpecNumber);

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

void AQLCharacterPlayer::GASInputReleased()
{
	uint8 InputAttackSpecNumber = static_cast<uint8>(CurrentAttackType);

	QL_LOG(QLNetLog, Log, TEXT("GAS current ID %d"), InputAttackSpecNumber);

	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputAttackSpecNumber);

	if (Spec)
	{
		Spec->InputPressed = false; //해당키를 눌렀음을 알려줌
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputReleased(*Spec);
		}
	}
}

void AQLCharacterPlayer::FarmingItemPressed()
{
	bPressedFarmingKey = true;
	//Raycast 를 사용해서 해당 오브젝트 파악하기
//	FVector Start = Get
	
}
void AQLCharacterPlayer::FarmingItemReleased()
{
	bPressedFarmingKey = false;
	//Raycast 를 사용해서 해당 오브젝트 파악하기
//	FVector Start = Get

}
void AQLCharacterPlayer::RunInputPressed()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	bIsRunning = true;

}

void AQLCharacterPlayer::RunInputReleased()
{
	if (IsLocallyControlled())
	{
		GetCharacterMovement()->MaxWalkSpeed = 400.f;
		bIsRunning = false;
	}
	//타이머를 통해서 MaxWalkSpeed를 줄이자	
}


void AQLCharacterPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y);

	AddControllerYawInput(LookAxisVector.X);

}

void AQLCharacterPlayer::Crunch()
{
	bIsCrunching = !bIsCrunching;
}

void AQLCharacterPlayer::Aim()
{
	if (bIsRunning) return;

	if (bHasGun)
	{
		QL_LOG(QLLog, Log, TEXT("Aim on"));
		bIsAiming = true;
		ZoomInTimeline->Play();
	}
}

void AQLCharacterPlayer::StopAiming()
{
	QL_LOG(QLLog, Log, TEXT("Aim off"));
	if (bIsAiming)
	{
		bIsAiming = false;
		ZoomInTimeline->ReverseFromEnd();
	}
}

void AQLCharacterPlayer::TimelineFloatReturn(float Alpha)
{
	float Length=FMath::Lerp(MaxArmLength, MinArmLength, Alpha);
	CameraSpringArm->TargetArmLength = Length;
}

