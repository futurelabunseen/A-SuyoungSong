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
#include "Item/QLBomb.h"
#include "Player/QLPlayerState.h"
#include "Player/QLPlayerController.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "Physics/QLCollision.h"
#include "GameData/QLGunStat.h"
#include "GameData/QLWeaponStat.h"
#include "GameData/QLAmmoData.h"
#include "Interface/QLGetItemStat.h"
#include "GameData/QLDataManager.h"
#include "Item/QLWeaponComponent.h"
#include "QLCharacterMovementComponent.h"
#include "Character/QLInputComponent.h"

#include "QuadLand.h"

AQLCharacterPlayer::AQLCharacterPlayer(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UQLCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)), bHasNextPunchAttackCombo(0), CurrentCombo(0), bPressedFarmingKey(0), MaxArmLength(300.0f),FarmingTraceDist(1000.0f)//, bIsTurning(false)
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

	// Mesh Component
	Weapon = CreateDefaultSubobject<UQLWeaponComponent>(TEXT("Weapon"));
	Weapon->Weapon->SetupAttachment(GetMesh(), TEXT("Gun"));
	QLInputComponent = CreateDefaultSubobject<UQLInputComponent>(TEXT("Input"));

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
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AQLCharacterPlayer::GetItem)));
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

	Weapon->Weapon->SetHiddenInGame(true); //Mesh 게임에서 안보이도록 해놓음

}

void AQLCharacterPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	
	QLInputComponent->InitPlayerImputComponent(PlayerInputComponent);
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
	if (bHasGun) return; 
	QL_LOG(QLLog, Warning, TEXT("Equip Weapon"));
	AQLItemBox* Item = Cast<AQLItemBox>(InItem);
	UQLItemData* InItemInfo = Item->Stat;
	//Mesh 위치는 소켓 
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

	UQLGunStat* GunStat = Cast<UQLGunStat>(WeaponStat);
	if (Weapon && GunStat->WeaponMesh)
	{
		if (GunStat->WeaponMesh.IsPending())
		{
			GunStat->WeaponMesh.LoadSynchronous();
		}
		Weapon->Weapon->SetHiddenInGame(false);
		Weapon->GroundWeapon = WeaponStat->GroundWeapon;
		//Weapon->Weapons.Add(WeaponStat->Type, WeaponStat); //제거하는 부분에서 제거해줘야함.
		Weapon->Weapon->SetSkeletalMesh(GunStat->WeaponMesh.Get());
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

	if (DataManager == nullptr)
	{
		return;
	}
	if (ItemStat)
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
	}
	int32 ItemCnt = --InventoryItem[InItemId]; //하나 사용

	QL_LOG(QLNetLog, Warning, TEXT("item use %d %d"), InItemId, InventoryItem[InItemId]);

	ClientRPCRemoveItem(ItemData, ItemCnt); //클라랑 서버랑 개수 일치
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

int AQLCharacterPlayer::GetInventoryCnt(EItemType ItemType)
{
	return InventoryItem[ItemType];
}

void AQLCharacterPlayer::MulticastRPCSwitchAttackType_Implementation(ECharacterAttackType InputKey)
{
	switch (InputKey)
	{
	case ECharacterAttackType::HookAttack:
		
		ASC->AddLooseGameplayTag(CHARACTER_EQUIP_NON);
		Weapon->Weapon->SetHiddenInGame(true); //총 숨김 (애니메이션도 풀어야함) => 이친구는,,,멀티캐스트 RPC 필요
		QL_LOG(QLLog, Warning, TEXT("this? 1"));
		if (Weapon->Bomb != nullptr)
		{
			QL_LOG(QLLog, Warning, TEXT("this? 2"));
			Weapon->SetBombHiddenInGame(true);
		}
		break;

	case ECharacterAttackType::GunAttack:
		ASC->AddLooseGameplayTag(CHARACTER_EQUIP_GUNTYPEA); //이것도 변경되어야할사항...
		Weapon->Weapon->SetHiddenInGame(false); //총 보여줌
		if (Weapon->Bomb != nullptr)
		{
			Weapon->SetBombHiddenInGame(true);
		}
		break;

	case ECharacterAttackType::BombAttack:
		ASC->AddLooseGameplayTag(CHARACTER_EQUIP_BOMB); //이것도 변경되어야할사항...
		Weapon->Weapon->SetHiddenInGame(true);
		Weapon->SetBombHiddenInGame(false);
		//총대신 교체,WeaponMesh 교체
		break;
	}
}

void AQLCharacterPlayer::ServerRPCSwitchAttackType_Implementation(ECharacterAttackType InputKey)
{

	AQLPlayerState* PS = Cast<AQLPlayerState>(GetPlayerState());

	if (PS == nullptr)
	{
		return;
	}


	UQLDataManager* DataManager = GetWorld()->GetSubsystem<UQLDataManager>();

	//ECharacterAttackType::BombAttack
	const UQLWeaponStat* WeaponStat = DataManager->GetWeaponStat(InputKey);

	if (WeaponStat)
	{
		PS->SetWeaponStat(WeaponStat);
	}

	if (InputKey == ECharacterAttackType::BombAttack)
	{
		if (Weapon->Bomb == nullptr)
		{
			Weapon->SpawnBomb();
		}
	}
	//여기서 스탯 조절

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
	if (ASC&& NewCount == 1)
	{
		ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_NON);
		ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_GUNTYPEA);
		CurrentAttackType = ECharacterAttackType::BombAttack;
	}
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


void AQLCharacterPlayer::UseItem(EItemType ItemId)
{
	if (EItemType::Ammo == ItemId || EItemType::Bomb == ItemId)
	{
		return; //얘는 사용할 수 없어;;
	}
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
			QL_LOG(QLNetLog, Warning, TEXT("Same Item %d"),InItemCnt);
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

	QL_LOG(QLNetLog, Warning, TEXT("Add Item %d"), ItemCnt);

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
				//인벤토리에 Item 정보를 전송
				UQLItemData* ItemData = CastChecked<UQLItemData>(HitItem->Stat);
				if (ItemData->ItemType == ItemId)
				{
					HitItem->SetLifeSpan(0.3f);
					//같으면 추가한다.
					if (InventoryItem.Find(ItemId))
					{
						IsNotFound = false;
						InventoryItem[ItemId] ++;
						QL_LOG(QLNetLog, Log, TEXT("Current Cnt %d"), InventoryItem[ItemId]);
					}

					if (ItemId == EItemType::Ammo)
					{
						AQLPlayerState* PS = CastChecked<AQLPlayerState>(GetPlayerState());
						IQLGetItemStat* ItemStat = CastChecked<IQLGetItemStat>(ItemData);
						PS->SetAmmoStat(ItemStat->GetStat());
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
	//Data매니저가 가지고 있는 초기값 만큼 Stat을 없앰
	for (int i = 0; i < RemainingItemCnt; i++)
	{
		FVector Location = GetActorLocation();
		Location.X += 5.0f * i;
		FActorSpawnParameters Params;
		Params.Owner = this;
		AQLItemBox* GroundItem = GetWorld()->SpawnActor<AQLItemBox>(DataManager->GetItemBoxClass(ItemId), Location, FRotator::ZeroRotator, Params);
		
		if (ItemId == EItemType::Ammo)
		{
			IQLGetItemStat* AmmoStatData = CastChecked<IQLGetItemStat>(DataManager->GetItem(ItemId));
			AQLPlayerState* PS = CastChecked<AQLPlayerState>(GetPlayerState());
			PS->BulletWaste(AmmoStatData->GetStat()); //땅에 버릴 때 
		}
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
	AQLItemBox* GroundItem = GetWorld()->SpawnActor<AQLItemBox>(Weapon->GroundWeapon, Location, FRotator::ZeroRotator, Params);
	CurrentAttackType = ECharacterAttackType::HookAttack;

	UQLDataManager* DataManager = GetWorld()->GetSubsystem<UQLDataManager>();
	AQLPlayerState* PS = CastChecked<AQLPlayerState>(GetPlayerState());
	//ECharacterAttackType::BombAttack
	const UQLWeaponStat* WeaponStat = DataManager->GetWeaponStat(CurrentAttackType);

	if (WeaponStat)
	{
		PS->SetWeaponStat(WeaponStat);
	}

	MulticastRPCPuttingWeapon();
}

void AQLCharacterPlayer::MulticastRPCPuttingWeapon_Implementation()
{
	ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_GUNTYPEA);
	ASC->AddLooseGameplayTag(CHARACTER_EQUIP_NON); //이것도 변경되어야할사항...
	//Reset
	QL_LOG(QLLog, Log, TEXT("Put Weapon"));
	Weapon->Weapon->SetSkeletalMesh(nullptr);
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


