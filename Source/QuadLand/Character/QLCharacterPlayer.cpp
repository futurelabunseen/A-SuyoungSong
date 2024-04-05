// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QLCharacterPlayer.h"
#include "GameFramework/SpringArmComponent.h" //springArm - GameFramework
#include "Camera/CameraComponent.h" //camera - Camera
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AttackActionData/QLPunchAttackData.h"
#include "Player/QLPlayerState.h"
#include "Player/QLPlayerController.h"
#include "AbilitySystemComponent.h"
#include "Physics/QLCollision.h"
#include "GameData/QLWeaponStat.h"
#include "Item/QLItemObject.h"
#include "Interface/ItemGettingInfoInterface.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "QuadLand.h"

AQLCharacterPlayer::AQLCharacterPlayer() : bIsFirstRunSpeedSetting(false), bHasNextPunchAttackCombo(0), CurrentCombo(0), bPressedFarmingKey(0), FarmingTraceDist(1000.0f)//, bIsTurning(false)
{
	bHasGun = false;
	ASC = nullptr;
	CurrentAttackType = ECharacterAttackType::HookAttack; //default

	//springArm에 Camera를 매달을 예정
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraSpringArm->TargetArmLength = 200.0f;
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

	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this,&AQLCharacterPlayer::EquipWeapon)));

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
	EnhancedInputComponent->BindAction(FarmingAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::FarmingItemPressed);
	EnhancedInputComponent->BindAction(FarmingAction, ETriggerEvent::Completed, this, &AQLCharacterPlayer::FarmingItemReleased);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::RunInputPressed);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AQLCharacterPlayer::RunInputReleased);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::JumpInputPressed);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AQLCharacterPlayer::JumpInputReleased);

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
			bPressedFarmingKey = false;

			OutHitResult.GetActor()->SetActorHiddenInGame(true);
			AQLItemObject* Item = Cast<AQLItemObject>(OutHitResult.GetActor());
			if (Item == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Item is not founded"));
				return;
			}
			TakeItemActions[static_cast<uint8>(Item->ItemType)].ItemDelegate.ExecuteIfBound(Item);
			UE_LOG(LogTemp, Log, TEXT("Item Get"));
		}
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
FVector AQLCharacterPlayer::CalPlayerLocalCameraStartPos()
{
	return  Camera->GetComponentLocation() + GetCameraForward() * CameraSpringArm->TargetArmLength;
}

FVector AQLCharacterPlayer::GetCameraForward()
{
	return  Camera->GetForwardVector();
}

const UQLWeaponStat* AQLCharacterPlayer::GetWeaponStat() const
{
	AQLPlayerState* PS = Cast<AQLPlayerState>(GetPlayerState());
	return PS->WeaponStat;
}

void AQLCharacterPlayer::EquipWeapon(AQLItemObject* InItemInfo)
{
	//CurrentAttackType = ECharacterAttackType::GunAttack;
	//Weapon 위치는 소켓 
	AQLPlayerState* PS = Cast<AQLPlayerState>(GetPlayerState());
	IItemGettingInfoInterface* ItemInfo = Cast< IItemGettingInfoInterface>(InItemInfo);
	PS->WeaponStat = ItemInfo->GetStat();
	if (PS->WeaponStat)
	{
		if (PS->WeaponStat->Mesh.IsPending())
		{
			PS->WeaponStat->Mesh.LoadSynchronous();
		}

		Weapon->SetStaticMesh(PS->WeaponStat->Mesh.Get());
	}
	//Mesh 를 생성해서 부착함.
	bHasGun = true;
	CurrentAttackType = ECharacterAttackType::GunAttack;

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

	ClearTurninPlace(MovementVector.X);
	ClearTurninPlace(MovementVector.Y);
}

void AQLCharacterPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);

	if (LookAxisVector.X != 0)
	{
		TurnInPlace();
	}
}

void AQLCharacterPlayer::GASInputPressed()
{
	if(bIsFirstRunSpeedSetting && CurrentAttackType == ECharacterAttackType::GunAttack)
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
	
	if (bIsFirstRunSpeedSetting == false&&IsLocallyControlled())
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
		bIsFirstRunSpeedSetting = true;
	}

}

void AQLCharacterPlayer::RunInputReleased()
{
	bIsFirstRunSpeedSetting = false;

	if (IsLocallyControlled())
	{
		GetCharacterMovement()->MaxWalkSpeed = 400.f;
	}
	//타이머를 통해서 MaxWalkSpeed를 줄이자	
}

void AQLCharacterPlayer::JumpInputPressed()
{
}

void AQLCharacterPlayer::JumpInputReleased()
{
}


void AQLCharacterPlayer::PlayTurn(UAnimMontage* TurnAnimMontage, float TurnRate, float TurnTimeDelay)
{
	if (bIsTurning == false)
	{
		bIsTurning = true;
		PlayAnimMontage(TurnAnimMontage, TurnRate);

		FTimerHandle TurnTimeHandle;
		GetWorld()->GetTimerManager().SetTimer(TurnTimeHandle, [=]() 
			{
			bIsTurning = false;
			}, TurnTimeDelay, false, -1);
	}
}
//enum값으로 변경 예정
void AQLCharacterPlayer::TurnLeft90()
{
	PlayTurn(TurnAnimMontages[0], 1.5f, 0.5f);
}

void AQLCharacterPlayer::TurnLeft180()
{
	PlayTurn(TurnAnimMontages[2], 1.7f, 0.6f);
}

void AQLCharacterPlayer::TurnRight90()
{
	PlayTurn(TurnAnimMontages[1], 1.5f, 0.5f);
}

void AQLCharacterPlayer::TurnRight180()
{
	PlayTurn(TurnAnimMontages[3], 1.7f, 0.6f);
}

void AQLCharacterPlayer::ClearMotion()
{
	if (IsPlayingRootMotion())
	{
		StopAnimMontage();
	}
}

void AQLCharacterPlayer::ClearTurninPlace(float Force)
{
	if (Force != 0.0f)
	{
		ClearMotion();
	}
}

void AQLCharacterPlayer::TurnInPlace()
{
	float GroundSpeed = GetCharacterMovement()->Velocity.Size2D();

	if (!(GetCharacterMovement()->IsFalling()) && !(GroundSpeed > 0.0f))
	{

		FRotator DeltaRotator(GetActorRotation() - GetBaseAimRotation());
		float Val = DeltaRotator.Yaw * -1.0f;

		bool RotationResult = (Val > 45.0f) || (Val < -45.f);

		if (RotationResult)
		{

			if (Val > 135.0f)
			{
				QL_LOG(QLLog, Log, TEXT("Current Rotator %f 1"), Val);
				TurnRight180();
			}
			else if (Val < -135.0f)
			{
				QL_LOG(QLLog, Log, TEXT("Current Rotator %f 2"), Val);
				TurnLeft180();
			}
			else if (Val > 45.0f)
			{
				QL_LOG(QLLog, Log, TEXT("Current Rotator %f 3"), Val);
				TurnRight90();
			}
			else if (Val < -45.0f)
			{
				QL_LOG(QLLog, Log, TEXT("Current Rotator %f 4"), Val);
				TurnLeft90();
			}
		}

	}
}