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
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Abilities/GameplayAbility.h"
#include "Components/SphereComponent.h"


#include "GameplayTag/GamplayTags.h"
#include "Item/QLItemBox.h"
#include "UI/QLUIType.h"
#include "Item/QLItem.h"
#include "Player/QLPlayerState.h"
#include "Player/QLPlayerController.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "Physics/QLCollision.h"
#include "GameData/QLWeaponStat.h"
#include "GameData/QLAmmoData.h"
#include "Interface/QLGetItemStat.h"
#include "GameData/QLDataManager.h"
#include "Item/QLWeaponComponent.h"
#include "QLCharacterMovementComponent.h"


#include "QuadLand.h"

AQLCharacterPlayer::AQLCharacterPlayer(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UQLCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)), bHasNextPunchAttackCombo(0), CurrentCombo(0), bPressedFarmingKey(0), FarmingTraceDist(1000.0f), MaxArmLength(300.0f)//, bIsTurning(false)
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

	CameraSpringArm->TargetArmLength = 300.f;
	CameraSpringArm->SetRelativeLocation(FVector(0.0f, 30.0f, 0.0f));
	CameraSpringArm->SocketOffset = FVector(0.0f, 15.0f, 48.0f);

	// Weapon Component
	Weapon = CreateDefaultSubobject<UQLWeaponComponent>(TEXT("Weapon"));
	Weapon->Weapon->SetupAttachment(GetMesh(), TEXT("Gun"));
	
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
		AttackAction = AttackActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> RunActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Run.IA_Run'"));

	if (RunActionRef.Object)
	{
		RunAction = RunActionRef.Object;
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
	static ConstructorHelpers::FObjectFinder<UInputAction> ReloadActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/QuadLand/Inputs/Action/IA_Reload.IA_Reload'"));

	if (ReloadActionRef.Object)
	{
		ReloadAction = ReloadActionRef.Object;
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

	bIsSetVisibleInventory = false;


	//InputContext Mapping
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputContextMappingRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/QuadLand/Inputs/IMC_Shoulder.IMC_Shoulder'"));

	if (InputContextMappingRef.Object)
	{
		InputMappingContext = InputContextMappingRef.Object;
	}
	

	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AQLCharacterPlayer::EquipWeapon)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AQLCharacterPlayer::HasLifeStone)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AQLCharacterPlayer::GetAmmo)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AQLCharacterPlayer::GetItem)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AQLCharacterPlayer::GetItem)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AQLCharacterPlayer::GetItem)));

	static ConstructorHelpers::FObjectFinder<UCurveFloat> AimCurveRef(TEXT("/Script/Engine.CurveFloat'/Game/QuadLand/Curve/AimAlpha.AimAlpha'"));

	if (AimCurveRef.Object)
	{
		AimAlphaCurve = AimCurveRef.Object;
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

	TakeItemDestory.BindUObject(this, &AQLCharacterPlayer::DestoryItem);
	TurningInPlace = ETurningPlaceType::ETIP_NotTurning;
	CurrentYaw = 0.0f;
	PreviousRotation = FRotator::ZeroRotator;
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
		SetupStartAbilities();
		//SetupGASInputComponent();

		AQLPlayerController* PC = Cast<AQLPlayerController>(NewController);

		if (PC)
		{
			PC->ConsoleCommand(TEXT("showdebug abilitysystem"));
			PC->CreateHUD();
		}
	}
	InitializeAttributes();
	ASC->RegisterGameplayTagEvent(CHARACTER_EQUIP_NON, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLCharacterPlayer::ResetNotEquip);
	ASC->RegisterGameplayTagEvent(CHARACTER_EQUIP_GUNTYPEA, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLCharacterPlayer::ResetEquipTypeA);
	ASC->RegisterGameplayTagEvent(CHARACTER_EQUIP_BOMB, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLCharacterPlayer::ResetBomb);

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

		//SetupGASInputComponent();
		QL_LOG(QLNetLog, Log, TEXT("Current Class is called by Client only"));
	}
	InitializeAttributes();

	AQLPlayerController* PC = Cast<AQLPlayerController>(GetController());
	if (PC)
	{
		PC->ConsoleCommand(TEXT("showdebug abilitysystem"));
		PC->CreateHUD();
	}
	ASC->RegisterGameplayTagEvent(CHARACTER_EQUIP_NON, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLCharacterPlayer::ResetNotEquip);
	ASC->RegisterGameplayTagEvent(CHARACTER_EQUIP_GUNTYPEA, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLCharacterPlayer::ResetEquipTypeA);
	ASC->RegisterGameplayTagEvent(CHARACTER_EQUIP_BOMB, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLCharacterPlayer::ResetBomb);

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

	ZoomInTimeline->AddInterpFloat(AimAlphaCurve, AimInterpFunction, FName{ TEXT("AimAlpha") });
	if (CameraDownTimeline && CameraUpDownCurve)
	{
		CameraDownTimeline->AddInterpFloat(CameraUpDownCurve, DownInterpFunction, FName{ TEXT("CameraDownAlpha") });
	}
	Weapon->Weapon->SetHiddenInGame(true); //Weapon 게임에서 안보이도록 해놓음

}

void AQLCharacterPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::Look);

	EnhancedInputComponent->BindAction(FarmingAction, ETriggerEvent::Started, this, &AQLCharacterPlayer::FarmingItemPressed);

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::JumpPressed);

	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::PressedCrouch);

	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &AQLCharacterPlayer::Aim);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AQLCharacterPlayer::StopAiming);

	EnhancedInputComponent->BindAction(PutLifeStoneAction, ETriggerEvent::Completed, this, &AQLCharacterPlayer::PutLifeStone);

	EnhancedInputComponent->BindAction(PutWeaponAction, ETriggerEvent::Completed, this, &AQLCharacterPlayer::PutWeapon);

	EnhancedInputComponent->BindAction(VisibilityInventoryAction, ETriggerEvent::Completed, this, &AQLCharacterPlayer::SetInventory);

	EnhancedInputComponent->BindAction(MapAction, ETriggerEvent::Completed, this, &AQLCharacterPlayer::SetMap);

	EnhancedInputComponent->BindAction(WeaponSwitcherAction[ECharacterAttackType::HookAttack], ETriggerEvent::Completed, this, &AQLCharacterPlayer::SelectDefaultAttackType);
	EnhancedInputComponent->BindAction(WeaponSwitcherAction[ECharacterAttackType::GunAttack], ETriggerEvent::Completed, this, &AQLCharacterPlayer::SelectGunAttackType);
	EnhancedInputComponent->BindAction(WeaponSwitcherAction[ECharacterAttackType::BombAttack], ETriggerEvent::Completed, this, &AQLCharacterPlayer::SelectBombAttackType);
	//PutLifeStone 
	SetupGASInputComponent();
}

void AQLCharacterPlayer::SetupGASInputComponent()
{
	if (IsValid(ASC) && IsValid(InputComponent))
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AQLCharacterPlayer::GASInputPressed, (int32)CurrentAttackType);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &AQLCharacterPlayer::GASInputReleased, (int32)CurrentAttackType);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AQLCharacterPlayer::GASInputPressed, 3);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Completed, this, &AQLCharacterPlayer::GASInputReleased,3);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &AQLCharacterPlayer::GASInputPressed, 4);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AQLCharacterPlayer::GASInputReleased, 4);
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


void AQLCharacterPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	RotateBornSetting(DeltaSeconds);
}

void AQLCharacterPlayer::FarmingItem()
{
	//드래그엔 드롭 방식은 이 파밍 아이템 사용 안할 예정. 
	bool bResult = false;
	FHitResult OutHitResult;

	FVector CameraLocStart = CalPlayerLocalCameraStartPos(); //카메라의 시작점 -> Spring Arm 만큼 앞으로 이동한 다음 물체가 있는지 확인

	FVector LocEnd = CameraLocStart + (GetCameraForward() * FarmingTraceDist);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ItemFarmingLineTrace), false, this); //식별자 

	bResult = GetWorld()->LineTraceSingleByChannel(
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
		if (bPressedFarmingKey) //꼭 무기라고 단정 지을 수는 없음. 
		{
			AQLItem* Item = Cast<AQLItem>(OutHitResult.GetActor());
			if (Item == nullptr || Item->Stat == nullptr)
			{
				QL_LOG(QLNetLog, Warning, TEXT("Item is not founded"));
				return;
			}
			UE_LOG(LogTemp, Warning, TEXT("Item Type %d"), Item->Stat->ItemType);

			TakeItemActions[static_cast<uint8>(Item->Stat->ItemType)].ItemDelegate.ExecuteIfBound(Item);
		}
	}
	//파밍키 타이머로 3초 뒤 해제 
}

void AQLCharacterPlayer::EquipWeapon(AQLItem* InItem)
{

	if (InItem == nullptr) return;
	QL_LOG(QLLog, Warning, TEXT("Equip Weapon"));
	AQLItemBox* Item = Cast<AQLItemBox>(InItem);
	UQLItemData* InItemInfo = Item->Stat;
	//Weapon 위치는 소켓 
	UQLWeaponStat* WeaponStat = CastChecked<UQLWeaponStat>(InItemInfo); //부착
	AQLPlayerState* PS = CastChecked<AQLPlayerState>(GetPlayerState());

	if (ASC->HasMatchingGameplayTag(CHARACTER_EQUIP_GUNTYPEA))
	{
		return; //있으면 리턴
	}
	if (HasAuthority())
	{
		CurrentAttackType = ECharacterAttackType::GunAttack;
		PS->SetWeaponStat(WeaponStat);
	}
	MulticastRPCFarming(WeaponStat);

	InItem->SetLifeSpan(0.5f);
}

void AQLCharacterPlayer::ServerRPCFarming_Implementation()
{
	if (bPressedFarmingKey) return;

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	FTimerHandle ItemMotionTimer;
	GetWorld()->GetTimerManager().SetTimer(ItemMotionTimer, FTimerDelegate::CreateLambda([&]()
		{
			bPressedFarmingKey = false;
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		}
	), 1.5f, false);

	bPressedFarmingKey = true;
	FarmingItem();
}

void AQLCharacterPlayer::MulticastRPCFarming_Implementation(UQLWeaponStat* WeaponStat)
{
	ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_NON);
	ASC->AddLooseGameplayTag(CHARACTER_EQUIP_GUNTYPEA); //이것도 변경되어야할사항...

	if (Weapon && WeaponStat->WeaponMesh)
	{
		if (WeaponStat->WeaponMesh.IsPending())
		{
			WeaponStat->WeaponMesh.LoadSynchronous();
		}
		Weapon->Weapon->SetHiddenInGame(false);
		Weapon->GroundWeapon = WeaponStat->GroundWeapon;
		Weapon->Weapons.Add(WeaponStat->Type, WeaponStat); //제거하는 부분에서 제거해줘야함.
		Weapon->Weapon->SetSkeletalMesh(WeaponStat->WeaponMesh.Get());
		bHasGun = true;
	}
}
void AQLCharacterPlayer::HasLifeStone(AQLItem* ItemInfo)
{
	AQLPlayerState* PS = CastChecked<AQLPlayerState>(GetPlayerState());

	FString ItemOwner = ItemInfo->GetOwner()->GetName(); //Owner -> PlayerState로 지정 ASC에 접근 가능하도록 변환
	FString CurrentGetOwner = PS->GetName();
	ItemInfo->SetActorEnableCollision(false);
	ItemInfo->SetActorHiddenInGame(true);
	ItemInfo->SetLifeSpan(3.f);

	if (ItemOwner == CurrentGetOwner)
	{
		PS->SetHasLifeStone(true);
		return;
	}

	//다르면 Dead 태그 부착
	UAbilitySystemComponent* ItemASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ItemInfo->GetOwner());

	if (ItemASC)
	{
		FGameplayTagContainer Tag(CHARACTER_STATE_DANGER);
		//ItemASC->AddLooseGameplayTag(CHARACTER_STATE_DANGER);
		ItemASC->TryActivateAbilitiesByTag(Tag);
		QL_LOG(QLLog, Warning, TEXT("TargetASC is Dead"));
	}
}

void AQLCharacterPlayer::GetAmmo(AQLItem* ItemInfo)
{
	UQLAmmoData* AmmoItem = Cast<UQLAmmoData>(ItemInfo->Stat);
	AQLPlayerState* PS = CastChecked<AQLPlayerState>(GetPlayerState());
	QL_LOG(QLNetLog, Log, TEXT("Get Ammo"));
	PS->SetAmmoStat(AmmoItem->AmmoCnt);
	GetItem(ItemInfo);
}
//Server Section
void AQLCharacterPlayer::GetItem(AQLItem* ItemInfo)
{
	UQLItemData* ItemData = Cast<UQLItemData>(ItemInfo->Stat);

	int32 ItemCnt = 1;
	if (!InventoryItem.Find(ItemData->ItemType))
	{
		InventoryItem.Add(ItemData->ItemType, ItemCnt);
	}
	else
	{
		ItemCnt = ++InventoryItem[ItemData->ItemType]; //없으면 0을 리턴
	}

	QL_LOG(QLNetLog, Warning, TEXT("Current Idx %s %d"), *ItemData->ItemName, ItemCnt);
	ClientRPCAddItem(ItemData, ItemCnt);

	ItemInfo->SetLifeSpan(0.5f);
}
//Client Section
void AQLCharacterPlayer::ClientRPCAddItem_Implementation(UQLItemData* ItemData, int32 ItemCnt)
{
	AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner());

	if (!InventoryItem.Find(ItemData->ItemType))
	{
		InventoryItem.Add(ItemData->ItemType, ItemCnt);
	}
	else
	{
		InventoryItem[ItemData->ItemType] = ItemCnt;
	}

	ItemData->CurrentItemCnt = ItemCnt;
	PC->UpdateItemEntry(ItemData, ItemCnt);
}

bool AQLCharacterPlayer::ServerRPCRemoveItem_Validate(EItemType ItemId, int32 ItemCnt)
{
	if (InventoryItem.Find(ItemId) == false)
	{
		QL_LOG(QLNetLog, Warning, TEXT("Index is currently exceeded"));
		return false;
	}

	if (InventoryItem[ItemId] != ItemCnt)
	{
		QL_LOG(QLNetLog, Warning, TEXT("The number of items does not match. %d %d"),InventoryItem[ItemId],ItemCnt);
		return false;
	}
	return true;
}
/* 버그 수정해라 */
void AQLCharacterPlayer::ServerRPCRemoveItem_Implementation(EItemType InItemId, int32 InItemCnt)
{
	QL_LOG(QLNetLog, Warning, TEXT("found a matching item %d %d"),InItemId, InventoryItem[InItemId]);

	AQLPlayerState* PS = CastChecked<AQLPlayerState>(GetPlayerState());
	UQLDataManager* DataManager = GetWorld()->GetSubsystem<UQLDataManager>();
	//아이템을 보관하고 있는 Manager 가져온다.

	//이부분 새로 생성해서 전달하도록 변경 했음.
	UQLItemData* ItemData = DataManager->GetItem(InItemId);

	IQLGetItemStat* ItemStat = Cast<IQLGetItemStat>(ItemData);

	if (ItemStat == nullptr)
	{
		//총알도 아이템으로 넣어놓자
		return;
	}
	if (DataManager)
	{
		switch (InItemId)
		{
		case EItemType::StaminaRecoveryItem:
			PS->AddStaminaStat(ItemStat->GetStat());
			break;
		case EItemType::HPRecoveryItem:
			PS->AddHPStat(ItemStat->GetStat());
			break;
		case EItemType::DiscoveryItem:
			PS->UseGlassesItem(ItemStat->GetStat());
			break;
		}
		int32 ItemCnt = --InventoryItem[InItemId]; //하나 사용
	
		QL_LOG(QLNetLog, Warning, TEXT("item use %d %d"), InItemId, InventoryItem[InItemId]);

		ClientRPCRemoveItem(ItemData, ItemCnt); //클라랑 서버랑 개수 일치
	}
}

void AQLCharacterPlayer::ClientRPCRemoveItem_Implementation(UQLItemData* Item, int32 ItemCnt)
{
	AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner());

	InventoryItem[Item->ItemType] = ItemCnt;
	Item->CurrentItemCnt = ItemCnt;
	PC->UpdateItemEntry(Item, ItemCnt);
	QL_LOG(QLNetLog, Warning, TEXT("update? %d %d"), Item->ItemType, InventoryItem[Item->ItemType]);
}

FVector AQLCharacterPlayer::CalPlayerLocalCameraStartPos()
{
	return  Camera->GetComponentLocation() + GetCameraForward() * CameraSpringArm->TargetArmLength;
}

FVector AQLCharacterPlayer::GetCameraForward()
{
	return  Camera->GetForwardVector();
}

void AQLCharacterPlayer::SelectDefaultAttackType()
{

	//총이 없어서 아무일도 하지않아도 Default임.
	if (Weapon->Weapon->bHiddenInGame || bHasGun == false) //숨겨져 아무것도 안들고 있다는 소리임.
	{
		return;
	}
	QL_LOG(QLLog, Warning, TEXT("Select Default Attack Type %d"));
	ServerRPCSwitchAttackType(ECharacterAttackType::HookAttack);

}

//총을 아예 먹지 않았을 수도 있음
void AQLCharacterPlayer::SelectGunAttackType()
{
	//총을 가지고 있지않으면, 총을 가질 수 없음.
	if (Weapon->Weapon->bHiddenInGame == false || bHasGun == false) //총쏘는 것이 일반적
	{
		return;
	}

	QL_LOG(QLLog, Warning, TEXT("Select Gun Attack Type"));
	ServerRPCSwitchAttackType(ECharacterAttackType::GunAttack);

}

void AQLCharacterPlayer::SelectBombAttackType()
{
	QL_LOG(QLLog, Warning, TEXT("Select Bomb Attack Type"));
	ServerRPCSwitchAttackType(ECharacterAttackType::BombAttack);
}

void AQLCharacterPlayer::MulticastRPCSwitchAttackType_Implementation(ECharacterAttackType InputKey)
{
	switch (InputKey)
	{
	case ECharacterAttackType::HookAttack:
		ASC->AddLooseGameplayTag(CHARACTER_EQUIP_NON);
		Weapon->Weapon->SetHiddenInGame(true); //총 숨김 (애니메이션도 풀어야함) => 이친구는,,,멀티캐스트 RPC 필요
		break;

	case ECharacterAttackType::GunAttack:
		ASC->AddLooseGameplayTag(CHARACTER_EQUIP_GUNTYPEA); //이것도 변경되어야할사항...
		Weapon->Weapon->SetHiddenInGame(false); //총 보여줌
		break;

	case ECharacterAttackType::BombAttack:
		ASC->AddLooseGameplayTag(CHARACTER_EQUIP_BOMB); //이것도 변경되어야할사항...
		//총대신 교체,WeaponMesh 교체
		break;
	}
	//CurrentAttackType = InputKey;
}

void AQLCharacterPlayer::ServerRPCSwitchAttackType_Implementation(ECharacterAttackType InputKey)
{
	MulticastRPCSwitchAttackType(InputKey);
}

void AQLCharacterPlayer::ResetNotEquip(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (ASC&& NewCount == 1)
	{
		ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_GUNTYPEA);
		ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_BOMB);
		CurrentAttackType = ECharacterAttackType::HookAttack;
	}
}

void AQLCharacterPlayer::ResetEquipTypeA(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (ASC&& NewCount == 1)
	{
		ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_NON);
		ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_BOMB);
		CurrentAttackType = ECharacterAttackType::GunAttack;
	}
}

void AQLCharacterPlayer::ResetBomb(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (ASC&& NewCount == 0)
	{
		ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_GUNTYPEA);
		ASC->AddLooseGameplayTag(CHARACTER_EQUIP_NON);
		CurrentAttackType = ECharacterAttackType::HookAttack;
		return;
	}
	ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_GUNTYPEA);
	ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_NON);
	CurrentAttackType = ECharacterAttackType::BombAttack;
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

void AQLCharacterPlayer::GASInputPressed(int32 id)
{

	uint8 InputAttackSpecNumber = GetInputNumber(id);

	if (CurrentAttackType == ECharacterAttackType::HookAttack && InputAttackSpecNumber == 3)
	{
		return;
	}

	QL_LOG(QLNetLog, Log, TEXT("begin %d"), CurrentAttackType);
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

void AQLCharacterPlayer::GASInputReleased(int32 id)
{
	uint8 InputAttackSpecNumber = GetInputNumber(id);

	QL_LOG(QLNetLog, Log, TEXT("begin %d"), InputAttackSpecNumber);
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

int8 AQLCharacterPlayer::GetInputNumber(int32 id)
{
	if (id < (int8)ECharacterAttackType::None)
	{
		return static_cast<uint8>(CurrentAttackType);
	}

	return id;
}

void AQLCharacterPlayer::FarmingItemPressed()
{
	//ServerRPC 호출
	ServerRPCFarming();

}

float AQLCharacterPlayer::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0; //Z는 점프축
	return Velocity.Size2D();
}

void AQLCharacterPlayer::RotateBornSetting(float DeltaTime)
{
	if (bUseControllerRotationYaw == false) return; //죽으면 Yaw 동작을 껐기 때문에 해당 함수 실행 안되도록 수행한다.

	//플레이어가 살아있으면 동작한다. -> 즉, 플레이어가 죽었으면 Yaw 동작을 끔
	float Speed = CalculateSpeed();
	bool IsFalling = GetMovementComponent()->IsFalling();
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
		TurnInPlace(DeltaTime);
	}

	if (Speed > 0.0f || IsFalling)
	{
		PreviousRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
		CurrentYaw = 0.f;
		TurningInPlace = ETurningPlaceType::ETIP_NotTurning;
	}
}

void AQLCharacterPlayer::TurnInPlace(float DeltaTime)
{
	//현재 Yaw>90.0f ->오른쪽
	//방향 외적 (+/-)
	if (CurrentYaw > 45.0f)
	{
		TurningInPlace = ETurningPlaceType::ETIP_Right;
	}
	else if (CurrentYaw < -45.0f)
	{
		TurningInPlace = ETurningPlaceType::ETIP_Left;
	}
	//Yaw<-90.0f ->왼쪽
	if (TurningInPlace != ETurningPlaceType::ETIP_NotTurning)
	{
		InterpYaw = FMath::FInterpTo(InterpYaw, 0.0f, DeltaTime, 4.0f); //도는 각도를 보간하고 있구나?
		CurrentYaw = InterpYaw;

		if (FMath::Abs(CurrentYaw) < 15.f) //어느정도 적당히 돌았음을 확인
		{
			TurningInPlace = ETurningPlaceType::ETIP_NotTurning;
			PreviousRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f); //Turn을 재조정
		}
	}
}


void AQLCharacterPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y);

	AddControllerYawInput(LookAxisVector.X);

}

void AQLCharacterPlayer::JumpPressed()
{
	if (bIsCrouched)
	{
		CameraDownTimeline->ReverseFromEnd();
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void AQLCharacterPlayer::SetupStartAbilities()
{

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

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
	}

	SetupGASInputComponent();
}

void AQLCharacterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AQLCharacterPlayer, bPressedFarmingKey);
	DOREPLIFETIME(AQLCharacterPlayer, bIsShooting);
	DOREPLIFETIME(AQLCharacterPlayer, bIsReload);
}

void AQLCharacterPlayer::DestoryItem(AQLItemBox* Item)
{
	if (Item)
	{
		Item->SetActorEnableCollision(false);
		Item->SetActorHiddenInGame(true);
		Item->Destroy(); //수정해야함.
	}
}

void AQLCharacterPlayer::PressedCrouch()
{
	//클라이언트에서만 크런치를 동작시킨다.
	if (bIsCrouched)
	{
		CameraDownTimeline->ReverseFromEnd();
		UnCrouch();
	}
	else
	{
		CameraDownTimeline->Play();
		Crouch();
	}
}

void AQLCharacterPlayer::TimelineCameraUpDownFloatReturn(float Alpha)
{
	float CameraHeight = FMath::Lerp(MaxCameraHeight, MinCameraHeight, Alpha);
	FVector OriginalSocketOffset = CameraSpringArm->SocketOffset;
	CameraSpringArm->SocketOffset = FVector(OriginalSocketOffset.X, OriginalSocketOffset.Y, CameraHeight);
}

void AQLCharacterPlayer::Aim()
{
	if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_RUN)) return;

	if (bIsUsingGun())
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
	float Length = FMath::Lerp(MaxArmLength, MinArmLength, Alpha);
	CameraSpringArm->TargetArmLength = Length;
}
void AQLCharacterPlayer::InitializeAttributes()
{
	if (!ASC)
	{
		return;
	}

	if (!DefaultAttributes) //Gameplay Effect를 통해서 모든 어트리뷰트 기본값으로 초기화
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

void AQLCharacterPlayer::PutLifeStone() //Ctrl -> 
{
	AQLPlayerState* PS = CastChecked<AQLPlayerState>(GetPlayerState());
	if (bIsNearbyBox)
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

void AQLCharacterPlayer::PutWeapon()
{
	if (HasAuthority())
	{
		CurrentAttackType = ECharacterAttackType::HookAttack;
	}
	ServerRPCPuttingWeapon();
}

void AQLCharacterPlayer::CheckBoxOverlap()
{
	if (bIsNearbyBox)
	{
		bIsNearbyBox = false;
	}
	else
	{
		bIsNearbyBox = true;
	}
}

void AQLCharacterPlayer::SetMap()
{
	AQLPlayerController* PlayerController = Cast<AQLPlayerController>(GetController());
	if (bIsVisibleMap == false)
	{
		PlayerController->SetVisibilityHUD(EHUDType::Map);
		bIsVisibleMap = true;
	}
	else
	{
		PlayerController->SetHiddenHUD(EHUDType::Map);
		bIsVisibleMap = false;
	}
}
//대박... 멍청한생각...이거... 서버로 안가구나...
void AQLCharacterPlayer::SetInventory()
{
	/*주변 아이템을 탐색하는 트레이스 적용*/
	AQLPlayerController* PlayerController = Cast<AQLPlayerController>(GetController());

	if (PlayerController == nullptr)
	{
		return;
	}
	bool bResult = false;
	FVector SearchLocation = GetMesh()->GetSocketLocation(FName("ItemDetectionSocket"));
	//서버에서만 적용
	FCollisionQueryParams Params(TEXT("DetectionItem"), false, this);

	TArray<FHitResult> NearbyItems;
	//ItemDetectionSocket
	bResult = GetWorld()->SweepMultiByChannel(
		NearbyItems,
		SearchLocation,
		SearchLocation,
		FQuat::Identity,
		CCHANNEL_QLITEMACTION,
		FCollisionShape::MakeSphere(SearchRange),
		Params
	);

	if (bResult)
	{
		for (const auto& NearbyItem : NearbyItems)
		{
			AQLItemBox* HitItem = Cast<AQLItemBox>(NearbyItem.GetActor());
			if (HitItem)
			{
				QL_LOG(QLNetLog, Log, TEXT("Item Name %s"), *HitItem->GetName());
				//인벤토리에 Item 정보를 전송
				UQLItemData* ItemData = CastChecked<UQLItemData>(HitItem->Stat);
				ItemData->CurrentItemCnt = 1;
				PlayerController->UpdateNearbyItemEntry(ItemData);

				//실제로 인벤토리에서 아이템을 옮기려고 할 때
				//1. 서버에도 주변 아이템으로 있는가? RPC 확인
				//2. 인벤토리로 드랍할 때 아이템 추가 - 클라이언트, 서버 모두 동시에 진행
			}
		}
	}
	
#if ENABLE_DRAW_DEBUG
	FColor Color = bResult ? FColor::Green : FColor::Red;
	DrawDebugSphere(GetWorld(), SearchLocation, SearchRange, 10.0f, Color, false, 5.0f);
#endif
	/*인벤토리가 켜지는 부분*/

	FInputModeUIOnly UIOnlyInputMode;

	PlayerController->SetVisibilityHUD(EHUDType::Inventory);
	PlayerController->bShowMouseCursor = true;
	PlayerController->SetInputMode(UIOnlyInputMode);
	
}

void AQLCharacterPlayer::UseItem(EItemType ItemId)
{
	if (InventoryItem.Find(ItemId))
	{
		ServerRPCRemoveItem(ItemId, InventoryItem[ItemId]);
	}
}

void AQLCharacterPlayer::AddInventoryByDraggedItem(EItemType InItemId,int32 InItemCnt)
{
	//같으면 추가한다.-> 이미 클라이언트에서는 있는 것을 확인했기때문에, 추가한다음에 서버에 검증을 요청

	if (!HasAuthority())
	{
		if (InventoryItem.Find(InItemId))
		{
			InventoryItem[InItemId] += InItemCnt;
			QL_LOG(QLNetLog, Warning, TEXT("Same Item"));
		}
		else
		{
			InventoryItem.Add(InItemId, InItemCnt);
		}

	}
	//실제로 아이템이 있는지 검사하기 위해서 서버에게 요청해야함;
	ServerRPCAddInventoryByDraggedItem(InItemId, InItemCnt);
	QL_LOG(QLNetLog, Warning, TEXT("AddItem %d"), InItemCnt);
}


void AQLCharacterPlayer::ServerRPCAddInventoryByDraggedItem_Implementation(EItemType ItemId, int32 ItemCnt)
{
	AQLPlayerController* PlayerController = Cast<AQLPlayerController>(GetController());

	if (PlayerController == nullptr)
	{
		return;
	}
	bool bResult = false;
	FVector SearchLocation = GetMesh()->GetSocketLocation(FName("ItemDetectionSocket"));
	//서버에서만 적용
	FCollisionQueryParams Params(TEXT("DetectionItem"), false, this);

	TArray<FHitResult> NearbyItems;
	//ItemDetectionSocket
	bResult = GetWorld()->SweepMultiByChannel(
		NearbyItems,
		SearchLocation,
		SearchLocation,
		FQuat::Identity,
		CCHANNEL_QLITEMACTION,
		FCollisionShape::MakeSphere(SearchRange),
		Params
	);

	if (bResult)
	{
		bool IsNotFound = true;
		for (const auto& NearbyItem : NearbyItems)
		{
			AQLItemBox* HitItem = Cast<AQLItemBox>(NearbyItem.GetActor());
			if (HitItem)
			{
				QL_LOG(QLNetLog, Log, TEXT("Item Name %s"), *HitItem->GetName());
				//인벤토리에 Item 정보를 전송
				UQLItemData* ItemData = CastChecked<UQLItemData>(HitItem->Stat);

				if (ItemData->ItemType == ItemId && ItemData->CurrentItemCnt == ItemCnt)
				{
					HitItem->SetLifeSpan(0.3f);
					//같으면 추가한다.
					if (InventoryItem.Find(ItemId))
					{
						IsNotFound = false;
						InventoryItem[ItemId] ++;
						QL_LOG(QLNetLog, Warning, TEXT("Same Item %d"), InventoryItem[ItemId]);
					}
				}
			}
		}

		if (IsNotFound)
		{
			InventoryItem.Add(ItemId, ItemCnt);
		}
	}
	else
	{

		QL_LOG(QLNetLog, Warning, TEXT("Not Found, Rollback"), ItemCnt);
		ClientRPCRollbackInventory(ItemId, ItemCnt);
	}

#if ENABLE_DRAW_DEBUG
	FColor Color = bResult ? FColor::Green : FColor::Red;
	DrawDebugSphere(GetWorld(), SearchLocation, SearchRange, 10.0f, Color, false, 5.0f);
#endif
}
void AQLCharacterPlayer::AddGroundByDraggedItem(EItemType ItemId, int32 ItemCnt)
{
	QL_LOG(QLNetLog, Warning, TEXT("AddGroundByDraggedItem %d"), ItemCnt);
	
	if (!HasAuthority())
	{
		if (InventoryItem.Find(ItemId))
		{
			InventoryItem[ItemId] -= ItemCnt;
		}
		else
		{
			QL_LOG(QLNetLog, Error, TEXT("Error / is not valid %d"), ItemCnt);
			return;
		}
	}
	//Server RPC 전송 -> Server 아이템 생성 및 아이템 조정 
	ServerRPCAddGroundByDraggedItem(ItemId, ItemCnt);
}

void AQLCharacterPlayer::ServerRPCAddGroundByDraggedItem_Implementation(EItemType ItemId, int32 ItemCnt)
{
	int RemainingItemCnt = 0;
	if (InventoryItem.Find(ItemId))
	{
		RemainingItemCnt = InventoryItem[ItemId];
		InventoryItem[ItemId] -= ItemCnt;
		QL_LOG(QLNetLog, Warning, TEXT("Current Server Item Cnt %d"), InventoryItem[ItemId]);
	}
	else
	{
		QL_LOG(QLNetLog, Error, TEXT("Error / is not valid %d"), ItemCnt);
		return;
	}

	UQLDataManager* DataManager = GetWorld()->GetSubsystem<UQLDataManager>();
	//아이템을 보관하고 있는 Manager 가져온다.

	//이부분 새로 생성해서 전달하도록 변경 했음.
	
	for (int i = 0; i < RemainingItemCnt; i++)
	{
		FVector Location = GetActorLocation();
		Location.X += 5.0f * i;
		FActorSpawnParameters Params;
		Params.Owner = this;
		AQLItemBox* GroundItem = GetWorld()->SpawnActor<AQLItemBox>(DataManager->GetItemBoxClass(ItemId), Location, FRotator::ZeroRotator, Params);
		
		QL_LOG(QLNetLog, Warning, TEXT("AddGroundByDraggedItem %d"), ItemCnt);
		//	GroundItem->bReplicate
	}	
}

void AQLCharacterPlayer::ClientRPCRollbackInventory_Implementation(EItemType InItemId, int32 ItemCnt)
{
	if (InventoryItem.Find(InItemId))
	{
		InventoryItem[InItemId] -= ItemCnt;
	}
}

bool AQLCharacterPlayer::ServerRPCPuttingWeapon_Validate()
{
	return bHasGun != false;
}

void AQLCharacterPlayer::ServerRPCPuttingWeapon_Implementation()
{
	// Multicast 위치 or Server 위치하고 Replicated할지.. 
	FVector Location = GetActorLocation();
	FActorSpawnParameters Params;
	AQLItemBox* GroundItem = GetWorld()->SpawnActor<AQLItemBox>(Weapon->GetStat(EWeaponType::TypeA)->GroundWeapon, Location, FRotator::ZeroRotator, Params);

	MulticastRPCPuttingWeapon();
}

void AQLCharacterPlayer::MulticastRPCPuttingWeapon_Implementation()
{
	ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_GUNTYPEA);
	ASC->AddLooseGameplayTag(CHARACTER_EQUIP_NON); //이것도 변경되어야할사항...

	AQLPlayerState* PS = CastChecked<AQLPlayerState>(GetPlayerState());
	//Reset
	QL_LOG(QLLog, Log, TEXT("Put Weapon"));
	Weapon->Weapon->SetSkeletalMesh(nullptr);
	PS->ResetWeaponStat(Weapon->GetStat(EWeaponType::TypeA));

	//Spawn 한다.
	Weapon->Weapons.Remove(EWeaponType::TypeA); //정리
	bHasGun = false;
}

void AQLCharacterPlayer::ServerRPCShooting_Implementation()
{
	//만약 Stat없으면 리턴 시켜
	bIsShooting = !bIsShooting;
}
void AQLCharacterPlayer::ServerRPCReload_Implementation()
{	//MaxAmmo 없으면 실행 불가능 

	bIsReload = !bIsReload;
}


