// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QLInputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h" 
#include "Engine/EngineTypes.h"

#include "Character/QLCharacterPlayer.h"
#include "Player/QLPlayerController.h"
#include "Player/QLPlayerState.h"
#include "GameplayTag/GamplayTags.h"
#include "Physics/QLCollision.h"
#include "GameData/QLItemData.h"
#include "Item/QLItemBox.h"
UQLInputComponent::UQLInputComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//EnhancedInput ����
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

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &UQLInputComponent::JumpPressed);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &UQLInputComponent::PressedCrouch);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &UQLInputComponent::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &UQLInputComponent::StopAiming);

		EnhancedInputComponent->BindAction(PutLifeStoneAction, ETriggerEvent::Completed, this, &UQLInputComponent::PutLifeStone);

		EnhancedInputComponent->BindAction(PutWeaponAction, ETriggerEvent::Completed, this, &UQLInputComponent::PutWeapon);

		EnhancedInputComponent->BindAction(VisibilityInventoryAction, ETriggerEvent::Completed, this, &UQLInputComponent::SetInventory);

		EnhancedInputComponent->BindAction(MapAction, ETriggerEvent::Completed, this, &UQLInputComponent::SetMap);


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
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &UQLInputComponent::GASInputPressed, 3);
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Completed, this, &UQLInputComponent::GASInputReleased, 3);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &UQLInputComponent::GASInputPressed, 4);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &UQLInputComponent::GASInputReleased, 4);
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
	//�̵� ����
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	AQLPlayerController* PC = GetController<AQLPlayerController>();

	if (Character && PC)
	{
		const FVector2D MovementVector = Value.Get<FVector2D>();

		const FRotator Rotation = PC->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0); //ȸ�� ������ Yaw�� ���д�
		//�� ���� - x��
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		//�� ���� - y��
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
		Character->AddControllerPitchInput(LookAxisVector.Y);
		Character->AddControllerYawInput(LookAxisVector.X);
	}
	
}


void UQLInputComponent::FarmingItemPressed()
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character)
	{
		Character->ServerRPCFarming();
	}

}

void UQLInputComponent::JumpPressed()
{

	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return;
	}

	if (Character->bIsCrouched)
	{
		CameraDownTimeline->ReverseFromEnd();
		Character->UnCrouch();
	}
	else
	{
		Character->Jump();
	}
}

void UQLInputComponent::PressedCrouch()
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return;
	}

	if (Character->bIsCrouched)
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

void AQLCharacterPlayer::InitializeAttributes()
{
	if (!ASC)
	{
		return;
	}

	if (!DefaultAttributes) //Gameplay Effect�� ���ؼ� ��� ��Ʈ����Ʈ �⺻������ �ʱ�ȭ
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = ASC->MakeOutgoingSpec(DefaultAttributes, 1, EffectContext);

	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), ASC.Get());
	}
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
	if (PS == nullptr || Character)
	{
		return;
	}
	if (Character->bIsNearbyBox)
	{
		PS->ServerRPCConcealLifeStone();
	}
	else
	{
		//������ Ŭ�� ��� �ش� ��ġ���� Spawn
		//�÷��̾��� ��ġ�� �����´�
		PS->ServerRPCPutLifeStone();
	}
}

void UQLInputComponent::PutWeapon()
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character)
	{
		return;
	}
	if (HasAuthority())
	{
		Character->CurrentAttackType = ECharacterAttackType::HookAttack;
	}
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

#if ENABLE_DRAW_DEBUG
	FColor Color = bResult ? FColor::Green : FColor::Red;
	DrawDebugSphere(GetWorld(), SearchLocation, Character->SearchRange, 10.0f, Color, false, 5.0f);
#endif
	/*�κ��丮�� ������ �κ�*/

	FInputModeUIOnly UIOnlyInputMode;

	PC->SetVisibilityHUD(EHUDType::Inventory);
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

	//���� ��� �ƹ��ϵ� �����ʾƵ� Default��.
	FGameplayTagContainer Tag(CHARACTER_EQUIP_NON);

	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();

	if (ASC->HasAnyMatchingGameplayTags(Tag))
	{
		return; //���� ��츸 üũ�ϸ�ȴ�.
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

	//���� ��� �ƹ��ϵ� �����ʾƵ� Default��.
	FGameplayTagContainer Tag(CHARACTER_EQUIP_GUNTYPEA);

	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (ASC->HasAnyMatchingGameplayTags(Tag) || Character->bHasGun == false)
	{
		return; //���� ��츸 üũ�ϸ�ȴ�.
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

	//���� ��� �ƹ��ϵ� �����ʾƵ� Default��.
	FGameplayTagContainer Tag(CHARACTER_EQUIP_BOMB);

	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (Character->InventoryItem.Find(EItemType::Bomb))
	{

		if (Character->InventoryItem[EItemType::Bomb] <= 0)
		{
			return;
		}
		Character->ServerRPCSwitchAttackType(ECharacterAttackType::BombAttack);
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

	if (Character->CurrentAttackType == ECharacterAttackType::HookAttack && InputAttackSpecNumber == 3)
	{
		return;
	}

	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputAttackSpecNumber);
	if (Spec)
	{

		Spec->InputPressed = true; //�ش�Ű�� �������� �˷��� 

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
		Spec->InputPressed = false; //�ش�Ű�� �������� �˷���
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
