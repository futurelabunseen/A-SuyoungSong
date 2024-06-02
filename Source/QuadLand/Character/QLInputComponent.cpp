// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QLInputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h" 
#include "Engine/EngineTypes.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"

#include "Character/QLCharacterPlayer.h"
#include "Character/QLCharacterMovementComponent.h"
#include "Player/QLPlayerController.h"
#include "Player/QLPlayerState.h"
#include "GameplayTag/GamplayTags.h"
#include "Physics/QLCollision.h"
#include "GameData/QLItemData.h"
#include "Item/QLItemBox.h"
#include "QuadLand.h"

UQLInputComponent::UQLInputComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), bShowMenuUI(false)
{
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

	static ConstructorHelpers::FObjectFinder<UInputAction> FarmingActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Faming.IA_Faming'"));

	if (FarmingActionRef.Object)
	{
		FarmingAction = FarmingActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Jump.IA_Jump'"));

	if (JumpActionRef.Object)
	{
		JumpAction = JumpActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> CrouchActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Crunch.IA_Crunch'"));

	if (CrouchActionRef.Object)
	{
		CrouchAction = CrouchActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> AimActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Aim.IA_Aim'"));

	if (AimActionRef.Object)
	{
		AimAction = AimActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MenuActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Menu.IA_Menu'"));

	if (MenuActionRef.Object)
	{
		MenuAction = MenuActionRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> PutLifeStoneActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_PutLifeStone.IA_PutLifeStone'"));

	if (PutLifeStoneActionRef.Object)
	{
		PutLifeStoneAction = PutLifeStoneActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> PutWeaponActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_PutWeapon.IA_PutWeapon'"));

	if (PutWeaponActionRef.Object)
	{
		PutWeaponAction = PutWeaponActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> VisibilityInventoryActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Inventory.IA_Inventory'"));

	if (VisibilityInventoryActionRef.Object)
	{
		VisibilityInventoryAction = VisibilityInventoryActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MapActionRep(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Map.IA_Map'"));

	if (MapActionRep.Object)
	{
		MapAction = MapActionRep.Object;
	}


	static ConstructorHelpers::FObjectFinder<UCurveFloat> AimCurveRef(TEXT("/Script/Engine.CurveFloat'/Game/QuadLand/Curve/AimAlpha.AimAlpha'"));

	if (AimCurveRef.Object)
	{
		AimAlphaCurve = AimCurveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> RunActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Run.IA_Run'"));

	if (RunActionRef.Object)
	{
		RunAction = RunActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> AttackActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Attack.IA_Attack'"));

	if (AttackActionRef.Object)
	{
		AttackAction = AttackActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ReloadActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Reload.IA_Reload'"));

	if (ReloadActionRef.Object)
	{
		ReloadAction = ReloadActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ProneActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Prone.IA_Prone'"));

	if (ProneActionRef.Object)
	{
		ProneAction = ProneActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ShootingMethodActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_ShootingMethod.IA_ShootingMethod'"));

	if (ShootingMethodActionRef.Object)
	{
		ShootingMethodAction = ShootingMethodActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UCurveFloat> CameraCurveRef(TEXT("/Script/Engine.CurveFloat'/Game/QuadLand/Curve/CameraDownAlpha.CameraDownAlpha'"));

	if (CameraCurveRef.Object)
	{
		CameraUpDownCurve = CameraCurveRef.Object;
	}
	ZoomInTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ZoominTimeline"));
	AimInterpFunction.BindUFunction(this, FName(TEXT("TimelineFloatReturn")));

	CameraDownTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("CameraDownTimeline"));
	DownInterpFunction.BindUFunction(this, FName(TEXT("TimelineCameraUpDownFloatReturn")));

}

void UQLInputComponent::InitPlayerImputComponent(UInputComponent* InputComponent)
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();

	if (Character)
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UQLInputComponent::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &UQLInputComponent::Look);

		EnhancedInputComponent->BindAction(FarmingAction, ETriggerEvent::Started, this, &UQLInputComponent::FarmingItemPressed);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &UQLInputComponent::PressedJump);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &UQLInputComponent::PressedCrouch);

		EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Started, this, &UQLInputComponent::ShowMenuUI);

		EnhancedInputComponent->BindAction(ProneAction, ETriggerEvent::Completed, this, &UQLInputComponent::PressedProne);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &UQLInputComponent::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &UQLInputComponent::StopAiming);

		EnhancedInputComponent->BindAction(PutLifeStoneAction, ETriggerEvent::Completed, this, &UQLInputComponent::PutLifeStone);

		EnhancedInputComponent->BindAction(PutWeaponAction, ETriggerEvent::Completed, this, &UQLInputComponent::PutWeapon);

		EnhancedInputComponent->BindAction(VisibilityInventoryAction, ETriggerEvent::Completed, this, &UQLInputComponent::SetInventory);

		EnhancedInputComponent->BindAction(MapAction, ETriggerEvent::Completed, this, &UQLInputComponent::SetMap);
		
		EnhancedInputComponent->BindAction(ShootingMethodAction, ETriggerEvent::Completed, this, &UQLInputComponent::ChangeShootingMethod);

		EnhancedInputComponent->BindAction(WeaponSwitcherAction[ECharacterAttackType::HookAttack], ETriggerEvent::Completed, this, &UQLInputComponent::SelectDefaultAttackType);
		EnhancedInputComponent->BindAction(WeaponSwitcherAction[ECharacterAttackType::GunAttack], ETriggerEvent::Completed, this, &UQLInputComponent::SelectGunAttackType);
		EnhancedInputComponent->BindAction(WeaponSwitcherAction[ECharacterAttackType::BombAttack], ETriggerEvent::Completed, this, &UQLInputComponent::SelectBombAttackType);
		
		InitGASInputComponent(InputComponent);

	}	
}

void UQLInputComponent::InitGASInputComponent(UInputComponent* InputComponent)
{

	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return;
	}

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &UQLInputComponent::GASInputPressed, (int32)Character->CurrentAttackType);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &UQLInputComponent::GASInputReleased, (int32)Character->CurrentAttackType);
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &UQLInputComponent::GASInputPressed, 4);
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Completed, this, &UQLInputComponent::GASInputReleased, 4);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &UQLInputComponent::GASInputPressed, 5);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &UQLInputComponent::GASInputReleased, 5);
}

void UQLInputComponent::BeginPlay()
{
	Super::BeginPlay();

	ZoomInTimeline->AddInterpFloat(AimAlphaCurve, AimInterpFunction, FName{ TEXT("AimAlpha") });
	if (CameraDownTimeline && CameraUpDownCurve)
	{
		CameraDownTimeline->AddInterpFloat(CameraUpDownCurve, DownInterpFunction, FName{ TEXT("CameraDownAlpha") });
	}
}

void UQLInputComponent::Move(const FInputActionValue& Value)
{
	//이동 벡터
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	AQLPlayerController* PC = GetController<AQLPlayerController>();

	if (Character && PC)
	{
		UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();

		FGameplayTagContainer TagContainer(CHARACTER_STATE_STOP);
		if (ASC->HasAnyMatchingGameplayTags(TagContainer))
		{
			ASC->RemoveLooseGameplayTags(TagContainer);
		}
		const FVector2D MovementVector = Value.Get<FVector2D>();

		const FRotator Rotation = PC->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0); //회전 방향인 Yaw만 냅둔다
		//앞 방향 - x축
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		//옆 방향 - y축
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		Character->AddMovementInput(ForwardDirection, MovementVector.X);
		Character->AddMovementInput(RightDirection, MovementVector.Y);

	}
}


void UQLInputComponent::Look(const FInputActionValue& Value)
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Character)
	{
		Character->AddControllerPitchInput(LookAxisVector.Y * 0.5f);
		Character->AddControllerYawInput(LookAxisVector.X * 0.5f);
	} 
	
}


void UQLInputComponent::FarmingItemPressed()
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character)
	{

		//라인트레이스를 쏘아보고 없으면 return
		FHitResult OutHitResult;

		FVector CameraLocStart = Character->CalPlayerLocalCameraStartPos(); //카메라의 시작점 -> Spring Arm 만큼 앞으로 이동한 다음 물체가 있는지 확인
		FVector LocEnd = CameraLocStart + (Character->GetCameraForward() * Character->FarmingTraceDist);

		FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(CheckItemCollision) , false, Character); //식별자 

		bool bResult =  GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			CameraLocStart,
			LocEnd,
			CCHANNEL_QLITEMACTION,
			CollisionParams
		);


		AQLPlayerController* PC = GetController<AQLPlayerController>();

		if (!PC)
		{
			return;
		}

		AQLItem* Item = Cast<AQLItem>(OutHitResult.GetActor());
		if (Item == nullptr || Item->Stat == nullptr)
		{
			return;
		}
		
		if (bResult)
		{

			if (Item->Stat->ItemType == EItemType::Stone)
			{
				PC->ConcealLifeStone();
			}

			if (Item->Stat->ItemType == EItemType::Weapon)
			{
				PC->UpdateEquipWeaponUI();
			}

			if (Item->Stat->ItemType == EItemType::Bomb)
			{
				PC->UpdateEquipBombUI();
			}

			Character->ServerRPCFarming();
		}
	}

}

void UQLInputComponent::PressedJump()
{

	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return;
	}


	if (Character->bIsProning)
	{
		PressedProne();
		return;
	}

	if (Character->bIsCrouched)
	{
		CameraDownTimeline->ReverseFromEnd();
		Character->UnCrouch();
		return;
	}

	Character->Jump();
}

void UQLInputComponent::PressedCrouch()
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return;
	}
	//현재 문제 Crouch -> Prone 
	UQLCharacterMovementComponent* Movement = Cast<UQLCharacterMovementComponent>(Character->GetMovementComponent());

	if (Movement->IsFalling())
	{
		return;
	}

	if (Character->bIsProning) //엎드려있는데 Crouch
	{
		return;
	}

	if (Character->bIsCrouched) //현재 Cro
	{
		CameraDownTimeline->ReverseFromEnd();
		Character->UnCrouch();
	}
	else
	{
		CameraDownTimeline->Play();
		Character->Crouch();
	}
}

void UQLInputComponent::PressedProne()
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return;
	}

	UQLCharacterMovementComponent* Movement = Cast<UQLCharacterMovementComponent>(Character->GetMovementComponent());

	if (Movement->IsFalling())
	{
		return;
	}

	if (Character->bIsCrouched)
	{
		Character->UnCrouch();
	}

	if (Character->bIsProning)
	{
		Character->PlayAnimMontage(ToStand); //Stand
		Character->bIsProning = false;
	}
	else
	{
		Character->PlayAnimMontage(ToProne);
		Character->bIsProning = true;
	}
	
	ServerRPCPressedProne();
}

void UQLInputComponent::MulticastRPCPressedProne_Implementation()
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return;
	}
	if (!Character->IsLocallyControlled())
	{
		if (Character->bIsCrouched)
		{
			Character->UnCrouch();
		}

		if (Character->bIsProning)
		{
			Character->PlayAnimMontage(ToStand); //Stand
			Character->bIsProning = false;
		}
		else
		{
			Character->PlayAnimMontage(ToProne);
			Character->bIsProning = true;
		}
	}
}


void UQLInputComponent::ServerRPCPressedProne_Implementation()
{
	MulticastRPCPressedProne();
}

void UQLInputComponent::TimelineCameraUpDownFloatReturn(float Alpha)
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return;
	}
	float CameraHeight = FMath::Lerp(MaxCameraHeight, MinCameraHeight, Alpha);
	FVector OriginalSocketOffset = Character->CameraSpringArm->SocketOffset;
	Character->CameraSpringArm->SocketOffset = FVector(OriginalSocketOffset.X, OriginalSocketOffset.Y, CameraHeight);
}

void UQLInputComponent::TimelineFloatReturn(float Alpha)
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return;
	}

	
	float Length = FMath::Lerp(Character->MaxArmLength, Character->MinArmLength, Alpha);
	Character->CameraSpringArm->TargetArmLength = Length;
}


void UQLInputComponent::Aim()
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return;
	}

	UAbilitySystemComponent *ASC = Character->GetAbilitySystemComponent();

	if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_RUN))
	{
		return;
	}

	if (Character->bIsUsingGun())
	{
		Character->bIsAiming = true;
		ZoomInTimeline->Play();
	}
}

void UQLInputComponent::StopAiming()
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return;
	}

	if (Character->bIsAiming)
	{
		Character->bIsAiming = false;
		ZoomInTimeline->ReverseFromEnd();
	}
}



void UQLInputComponent::PutLifeStone()
{
	AQLPlayerState* PS = GetPlayerState<AQLPlayerState>();
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (PS == nullptr || Character == nullptr)
	{
		return;
	}
	AQLPlayerController* PC = GetController<AQLPlayerController>(); //소유권은 PC가 가짐

	if (PC && PS->GetHasLifeStone())
	{
		PC->ConcealLifeStone();
	}

	if (Character->bIsNearbyBox)
	{
		PS->ServerRPCConcealLifeStone();
	}
	else
	{
		//서버와 클라 모두 해당 위치에서 Spawn
		//플레이어의 위치를 가져온다
		PS->ServerRPCPutLifeStone();
	}
}

void UQLInputComponent::PutWeapon()
{

	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	AQLPlayerController* PC = GetController<AQLPlayerController>();
	if (Character == nullptr || PC == nullptr)
	{
		return;
	}
	if (Character->bHasGun == false)
	{
		return;
	}

	if (HasAuthority())
	{
		Character->CurrentAttackType = ECharacterAttackType::HookAttack;
	}

	PC->UpdateEquipWeaponUI();
	Character->ServerRPCPuttingWeapon();
}

void UQLInputComponent::SetInventory()
{

	AQLPlayerController* PC = GetController<AQLPlayerController>();
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (PC == nullptr || Character == nullptr)
	{
		return;
	}

	FVector SearchLocation = Character->GetMesh()->GetSocketLocation(FName("ItemDetectionSocket"));

	FCollisionQueryParams Params(TEXT("DetectionItem"), false, Character);

	TArray<FHitResult> NearbyItems;

	bool bResult = GetWorld()->SweepMultiByChannel(
		NearbyItems,
		SearchLocation,
		SearchLocation,
		FQuat::Identity,
		CCHANNEL_QLITEMACTION,
		FCollisionShape::MakeSphere(Character->SearchRange),
		Params
	);

	if (bResult)
	{
		for (const auto& NearbyItem : NearbyItems)
		{
			AQLItemBox* HitItem = Cast<AQLItemBox>(NearbyItem.GetActor());
			if (HitItem)
			{
				UQLItemData* ItemData = CastChecked<UQLItemData>(HitItem->Stat);
				if (ItemData->ItemType != EItemType::Weapon)
				{
					ItemData->CurrentItemCnt = 1;
					PC->UpdateNearbyItemEntry(ItemData);
				}
			}
		}
	}
	Character->UpdateAmmoUI();

	FInputModeUIOnly UIOnlyInputMode;
	PC->SetVisibilityHUD(EHUDType::Inventory);
	Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PC->bShowMouseCursor = true;
	PC->SetInputMode(UIOnlyInputMode);
}


void UQLInputComponent::SetMap()
{
	AQLPlayerController* PC = GetController<AQLPlayerController>();
	if (bIsVisibleMap)
	{
		PC->SetHiddenHUD(EHUDType::Map);
		bIsVisibleMap = false;
	}
	else
	{
		PC->SetVisibilityHUD(EHUDType::Map);
		bIsVisibleMap = true;
	}
}


void UQLInputComponent::SelectDefaultAttackType()
{

	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();

	if (Character == nullptr)
	{
		return;
	}

	//총이 없어서 아무일도 하지않아도 Default임.
	FGameplayTagContainer Tag(CHARACTER_EQUIP_NON);

	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();

	if (ASC->HasAnyMatchingGameplayTags(Tag))
	{
		return; //같은 경우만 체크하면된다.
	}

	
	Character->ServerRPCSwitchAttackType(ECharacterAttackType::HookAttack);
}


void UQLInputComponent::SelectGunAttackType()
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return;
	}

	//총이 없어서 아무일도 하지않아도 Default임.
	FGameplayTagContainer Tag(CHARACTER_EQUIP_GUNTYPEA);

	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (ASC->HasAnyMatchingGameplayTags(Tag) || Character->bHasGun == false)
	{
		return; //같은 경우만 체크하면된다.
	}
	
	Character->ServerRPCSwitchAttackType(ECharacterAttackType::GunAttack);
}

void UQLInputComponent::SelectBombAttackType()
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return;
	}

	//총이 없어서 아무일도 하지않아도 Default임.
	FGameplayTagContainer Tag(CHARACTER_EQUIP_BOMB);
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();

	if (ASC->HasAnyMatchingGameplayTags(Tag) || Character->GetInventoryCnt(EItemType::Bomb) == 0)
	{
		return;
	}
	Character->ServerRPCSwitchAttackType(ECharacterAttackType::BombAttack);

}


void UQLInputComponent::ChangeShootingMethod()
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	AQLPlayerController* PC = GetController<AQLPlayerController>();
	if (Character == nullptr||Character->bHasGun == false)
	{
		return;
	}
	
	if (Character->CurrentAttackType == ECharacterAttackType::GunAttack)
	{
		Character->bIsSemiAutomatic = false;
	}
	else
	{
		Character->bIsSemiAutomatic = true;
	}

	Character->OnChangeShootingMethod.ExecuteIfBound(Character->bIsSemiAutomatic,false);
	ServerRPCChangeShootingMethod();
}

void UQLInputComponent::ServerRPCChangeShootingMethod_Implementation()
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return;
	}
	if (Character->CurrentAttackType == ECharacterAttackType::GunAttack)
	{
		Character->CurrentAttackType = ECharacterAttackType::AutomaticGunAttack;
		Character->bIsSemiAutomatic = false;
	}
	else
	{
		Character->CurrentAttackType = ECharacterAttackType::GunAttack;
		Character->bIsSemiAutomatic = true;
	}
}

void UQLInputComponent::GASInputPressed(int32 id)
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	
	if (Character == nullptr)
	{
		return;
	}
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	uint8 InputAttackSpecNumber = GetInputNumber(id);

	if (Character->CurrentAttackType == ECharacterAttackType::HookAttack && InputAttackSpecNumber == 4)
	{
		return;
	}

	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputAttackSpecNumber);
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

void UQLInputComponent::GASInputReleased(int32 id)
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();

	if (Character == nullptr)
	{
		return;
	}
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	uint8 InputAttackSpecNumber = GetInputNumber(id);

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

int8 UQLInputComponent::GetInputNumber(int32 id)
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return -1;
	}

	if (id < (int8)ECharacterAttackType::None)
	{
		return static_cast<uint8>(Character->CurrentAttackType);
	}

	return id;
}

void UQLInputComponent::ShowMenuUI()
{
	AQLPlayerController* PC = GetController<AQLPlayerController>();
	if (PC == nullptr)
	{
		return;
	}

	if (bShowMenuUI == false)
	{
		FInputModeUIOnly UIOnlyInputMode;
		PC->SetInputMode(UIOnlyInputMode);
		PC->SetShowMouseCursor(true);
		PC->SetVisibilityHUD(EHUDType::Menu);
	}
	else
	{
		PC->CloseHUD(EHUDType::Menu);
		PC->SetHiddenHUD(EHUDType::Menu);
	}
	bShowMenuUI = !bShowMenuUI;

	QL_SUBLOG(QLLog, Warning, TEXT("!!!!! %d"), bShowMenuUI);
}

void UQLInputComponent::SetShowMenuUI()
{
	AQLPlayerController* PC = GetController<AQLPlayerController>();
	if (PC == nullptr)
	{
		return;
	}

	if (bShowMenuUI)
	{
		PC->CloseHUD(EHUDType::Menu);
		PC->SetHiddenHUD(EHUDType::Menu);
		bShowMenuUI = false;
	}

}