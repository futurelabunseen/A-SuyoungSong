// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QLCharacterPlayer.h"
#include "GameFramework/SpringArmComponent.h" //springArm - GameFramework
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h" //camera - Camera
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "AbilitySystemComponent.h"
#include "EngineUtils.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Abilities/GameplayAbility.h"
#include "Components/SplineComponent.h"
#include "Components/CapsuleComponent.h"

#include "GameplayTag/GamplayTags.h"
#include "Item/QLItemBox.h"
#include "UI/QLUIType.h"
#include "Item/QLItem.h"
#include "Item/QLBomb.h"
#include "Animation/QLAnimInstance.h"
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
#include "Character/QLInventoryComponent.h"
#include "Game/QLGameMode.h"
#include "UI/QLNicknameWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "QuadLand.h"

AQLCharacterPlayer::AQLCharacterPlayer(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UQLCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)), bHasNextPunchAttackCombo(0), CurrentCombo(0),  MaxArmLength(300.0f), bPressedFarmingKey(0), FarmingTraceDist(200.f)//, bIsTurning(false)
{
	bHasGun = false;
	ASC = nullptr;
	MovingThreshold = 3.0f;
	CurrentAttackType = ECharacterAttackType::HookAttack; //default

	ProneMeshLoc = FVector(0.f,0.f,-30.f);
	ProneCapsuleRadius = OriginalCapsuleRadius;
	ProneCapsuleHeight = OriginalCapsuleRadius;


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

	QLInputComponent = CreateDefaultSubobject<UQLInputComponent>(TEXT("Input"));
	QLInventory = CreateDefaultSubobject<UQLInventoryComponent>(TEXT("Inventory"));

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
	bIsSemiAutomatic = true;

	static ConstructorHelpers::FObjectFinder<UCurveFloat> HorizontalRecoilRef(TEXT("/Script/Engine.CurveFloat'/Game/QuadLand/Curve/HorizontalRecoilCurve.HorizontalRecoilCurve'"));

	if (HorizontalRecoilRef.Object)
	{
		HorizontalRecoil = HorizontalRecoilRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UCurveFloat> VerticalRecoilRef(TEXT("/Script/Engine.CurveFloat'/Game/QuadLand/Curve/VerticalRecoilCurve.VerticalRecoilCurve'"));

	if (VerticalRecoilRef.Object)
	{
		VerticalRecoil = VerticalRecoilRef.Object;
	}
	//AnimInstance
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceRef(TEXT("/Game/QuadLand/Animations/Blueprint/ABQL_Character.ABQL_Character_C"));

	if (AnimInstanceRef.Class)
	{
		GetMesh()->SetAnimClass(AnimInstanceRef.Class);
	}

	BombPath = CreateDefaultSubobject<USplineComponent>(TEXT("BombPath"));
	BombPath->SetupAttachment(GetMesh(), TEXT("Bomb"));
	BombPath->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	BombPath->SetHiddenInGame(true); //Bomb을 들지않았을 때에는 보이지않는다.
}
/// <summary>
/// PossessedBy 자체가 서버에서만 호출되기 때문에, 아래 Ability System 등록은 서버에서만 수행
/// </summary>
/// <param name="NewController"></param>
void AQLCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	SetupStartAbilities();
	InitializeGAS();
	
	if (ASC)
	{
		ASC->RegisterGameplayTagEvent(CHARACTER_EQUIP_NON, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLCharacterPlayer::ResetNotEquip);
		ASC->RegisterGameplayTagEvent(CHARACTER_EQUIP_GUNTYPEA, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLCharacterPlayer::ResetEquipTypeA);
		ASC->RegisterGameplayTagEvent(CHARACTER_EQUIP_BOMB, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLCharacterPlayer::ResetBomb);
		ASC->RegisterGameplayTagEvent(CHARACTER_STATE_RELOAD, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLCharacterPlayer::UpdateAmmo);
	}

	InitNickname();	

	GetWorld()->GetTimerManager().SetTimerForNextTick
	(
		[NewController]()
		{
			AQLPlayerController* PC = Cast<AQLPlayerController>(NewController);

			if (PC)
			{
				PC->InitWidget();
			}
		}
	);
	
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
	InitializeGAS();
	if (ASC)
	{
		ASC->RegisterGameplayTagEvent(CHARACTER_EQUIP_NON, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLCharacterPlayer::ResetNotEquip);
		ASC->RegisterGameplayTagEvent(CHARACTER_EQUIP_GUNTYPEA, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLCharacterPlayer::ResetEquipTypeA);
		ASC->RegisterGameplayTagEvent(CHARACTER_EQUIP_BOMB, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLCharacterPlayer::ResetBomb);
		ASC->RegisterGameplayTagEvent(CHARACTER_STATE_RELOAD, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLCharacterPlayer::UpdateAmmo);
	}

	TObjectPtr<AQLCharacterPlayer> Character = this;
	GetWorld()->GetTimerManager().SetTimerForNextTick
	(
		[Character]()
		{
			Character->InitNickname();
			AQLPlayerController* PC = Character->GetController<AQLPlayerController>();
			if (PC)
			{
				PC->ClientRPCCreateWidget();
			}
		}
	);
	
}

void AQLCharacterPlayer::OnRep_Controller()
{

	Super::OnRep_Controller();

	AQLPlayerController* PlayerController = Cast<AQLPlayerController>(GetController());

	if (PlayerController->HUDNum() == 0)
	{
		PlayerController->CreateHUD();
	}

	TObjectPtr<AQLCharacterPlayer> Character = this;
	GetWorld()->GetTimerManager().SetTimerForNextTick
	(
		[Character]()
		{
			Character->InitNickname();
		}
	);
}


void AQLCharacterPlayer::BeginPlay()
{

	Super::BeginPlay();

	AQLPlayerController* PlayerController = Cast<AQLPlayerController>(GetController());
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}

	SetCharacterControl();

	Weapon->Weapon->SetHiddenInGame(true); //Mesh 게임에서 안보이도록 해놓음

	GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.AddDynamic(this, &AQLCharacterPlayer::OnPlayMontageNotifyBegin);
	GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &AQLCharacterPlayer::OnPlayMontageNotifyEnd);
	if (!HorizontalRecoil || !VerticalRecoil)
	{
		return;
	}

	FOnTimelineFloat XRecoilCurve;
	//델리게이트 연결
	XRecoilCurve.BindUFunction(this, FName("StartHorizontalRecoil"));
	FOnTimelineFloat YRecoilCurve;
	YRecoilCurve.BindUFunction(this, FName("StartVerticalRecoil"));

	RecoilTimeline.AddInterpFloat(HorizontalRecoil, XRecoilCurve);
	RecoilTimeline.AddInterpFloat(VerticalRecoil, YRecoilCurve);

	StartHeight = GetActorLocation().Z;

	StandMeshLoc = GetMesh()->GetRelativeLocation();
	GetCapsuleComponent()->GetUnscaledCapsuleSize(OriginalCapsuleRadius, OriginalCapsuleHeight);
}

void AQLCharacterPlayer::InitializeGAS()
{
	if (!ASC)
	{
		return;
	}

	if (!DefaultEffects) //Gameplay Effect를 통해서 모든 어트리뷰트 기본값으로 초기화
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = ASC->MakeOutgoingSpec(DefaultEffects, 1, EffectContext);

	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), ASC.Get());
	}

	FGameplayTagContainer TagContainer(CHARACTER_EQUIP_NON);
	TagContainer.AddTag(CHARACTER_EQUIP_GUNTYPEA);
	TagContainer.AddTag(CHARACTER_EQUIP_BOMB);
	ASC->RemoveLooseGameplayTags(TagContainer); //모두 초기화
	ASC->AddLooseGameplayTag(CHARACTER_EQUIP_NON);
	
	AQLPlayerController* PC = GetController<AQLPlayerController>();
	if (PC)
	{
		PC->ResetUI();
	}
}

void AQLCharacterPlayer::ServerInitializeGAS()
{

}


void AQLCharacterPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);	

	QLInputComponent->InitPlayerImputComponent(PlayerInputComponent);
}

void AQLCharacterPlayer::SetCharacterControl()
{

	AQLPlayerController* PlayerController = Cast<AQLPlayerController>(GetController());

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

		if (HasAuthority() && PlayerController->HUDNum() == 0)
		{
			PlayerController->CreateHUD();
			PlayerController->ClientRPCCreateWidget();
		}
	}

}


void AQLCharacterPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (RecoilTimeline.IsPlaying())
	{
		RecoilTimeline.TickTimeline(DeltaSeconds); //앞으로 진행
	}

	if (RecoilTimeline.IsReversing())
	{
		RecoilTimeline.TickTimeline(DeltaSeconds); //되돌리기
	}

}



bool AQLCharacterPlayer::IsMontagePlaying(UAnimMontage* Montage) const
{
	UQLAnimInstance *AnimInstance = Cast<UQLAnimInstance>(GetMesh()->GetAnimInstance());

	if (AnimInstance)
	{
		return AnimInstance->Montage_IsPlaying(Montage);
	}

	return false;
}

void AQLCharacterPlayer::ClientRPCThrowBomb_Implementation()
{
	ThrowBomb = false;
}

void AQLCharacterPlayer::FarmingItem()
{
	//드래그엔 드롭 방식은 이 파밍 아이템 사용 안할 예정. 
	bool bResult = false;
	FHitResult OutHitResult;

	FVector CameraLocStart = CalPlayerLocalCameraStartPos(); //카메라의 시작점 -> Spring Arm 만큼 앞으로 이동한 다음 물체가 있는지 확인

	FVector LocEnd = CameraLocStart + (GetCameraForward() * FarmingTraceDist);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ItemFarmingLineTrace), false, this); //식별자 
	bResult = GetWorld()->SweepSingleByChannel(
		OutHitResult,
		CameraLocStart,
		LocEnd,
		FQuat::Identity,
		CCHANNEL_QLITEMACTION,
		FCollisionShape::MakeSphere(30.0f),
		Params);


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
			TakeItemActions[static_cast<uint8>(Item->Stat->ItemType)].ItemDelegate.ExecuteIfBound(Item);
		}
	}
	//파밍키 타이머로 3초 뒤 해제 
}

void AQLCharacterPlayer::EquipWeapon(AQLItem* InItem)
{

	if (InItem == nullptr) return;
	if (bHasGun) return; 

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
	//몽타주 실행
	
	if (IsMontagePlaying(PickupMontage) == false)
	{
		PlayAnimMontage(PickupMontage);
	}

	//모든 클라이언트에게 애니메이션 실행과정을 전달
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController && GetController() != PlayerController)
		{
			if (!PlayerController->IsLocalController())
			{
				AQLCharacterPlayer* OtherPlayer = Cast<AQLCharacterPlayer>(PlayerController->GetPawn());
				if (OtherPlayer)
				{
					OtherPlayer->ClientRPCPlayAnimation(this);
				}
			}
		}
	}
	bPressedFarmingKey = true;
	FarmingItem(); //실제 파밍한 아이템 
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
		Weapon->Weapon->SetSkeletalMesh(GunStat->WeaponMesh.Get());
		bHasGun = true;
		//isLocal로 잡아서 여기서 true 시키자 아!

		if (IsLocallyControlled())
		{
			AQLPlayerController* PC = CastChecked<AQLPlayerController>(GetController());
			PC->UpdateEquipWeaponUI(true);
		}
	}
	
}

void AQLCharacterPlayer::HasLifeStone(AQLItem* ItemInfo)
{
	AQLPlayerState* PS = CastChecked<AQLPlayerState>(GetPlayerState());

	AQLPlayerState* ItemOwner = Cast<AQLPlayerState>(ItemInfo->GetOwner());
	if (ItemOwner == nullptr)
	{
		return;
	}

	FString ItemOwnerName = ItemOwner->GetName(); //Owner -> PlayerState로 지정 ASC에 접근 가능하도록 변환
	FString CurrentGetOwnerName = PS->GetName();
	ItemInfo->SetActorEnableCollision(false);
	ItemInfo->SetActorHiddenInGame(true);
	ItemInfo->SetLifeSpan(3.f);

	if (ItemOwnerName == CurrentGetOwnerName)
	{
		PS->SetHasLifeStone(true);
		return;
	}

	//다르면 Dead 태그 부착
	UAbilitySystemComponent* ItemASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ItemOwner);

	if (ItemASC)
	{
		FGameplayTagContainer Tag(CHARACTER_STATE_DANGER);
		ItemASC->TryActivateAbilitiesByTag(Tag);
	}
}

void AQLCharacterPlayer::GetAmmo(AQLItem* ItemInfo)
{

	UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());

	if (DataManager)
	{
		UQLItemData* ItemData = DataManager->GetItem(EItemType::Ammo);

		UQLAmmoData* AmmoItem = Cast<UQLAmmoData>(ItemData);
		AQLPlayerState* PS = CastChecked<AQLPlayerState>(GetPlayerState());
		PS->SetAmmoStat(AmmoItem->AmmoCnt);
		GetItem(ItemInfo);
	}
	
}

FVector AQLCharacterPlayer::CalPlayerLocalCameraStartPos()
{
	return  Camera->GetComponentLocation() + GetCameraForward() * CameraSpringArm->TargetArmLength;
}


FVector AQLCharacterPlayer::GetCameraForward()
{
	return  Camera->GetForwardVector();
}

void AQLCharacterPlayer::UpdateAmmoTemp()
{
	if (QLInventory && QLInventory->InventoryItem.Find(EItemType::Ammo))
	{
		QLInventory->InventoryItem[EItemType::Ammo] = 0;
		ClientRPCUpdateAmmoUI();
	}
}


void AQLCharacterPlayer::ClientRPCUpdateAmmoUI_Implementation()
{
	UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());
	
	if (DataManager)
	{
		UQLItemData* ItemData = DataManager->GetItem(EItemType::Ammo);
		AQLPlayerController* PC = CastChecked<AQLPlayerController>(GetController());

		PC->UpdateItemEntry(ItemData, 0);
		QLInventory->InventoryItem[EItemType::Ammo] = 0;
	}
	
}


void AQLCharacterPlayer::UpdateAmmo(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (QLInventory->GetInventoryCnt(EItemType::Ammo))
	{
		AQLPlayerState* PS = CastChecked<AQLPlayerState>(GetPlayerState());
		AQLPlayerController* PC = CastChecked<AQLPlayerController>(GetController());

		UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());

		UQLItemData* ItemData = DataManager->GetItem(EItemType::Ammo);

		uint32 ItemCnt = FMath::RoundToInt((PS->GetMaxAmmoCnt() + PS->GetCurrentAmmoCnt()) / PS->GetAmmoCnt());
	
		if (QLInventory->GetInventoryCnt(EItemType::Ammo))
		{
			QLInventory->InventoryItem[EItemType::Ammo] = ItemCnt;
		}

		if (IsLocallyControlled())
		{
			PC->UpdateItemEntry(ItemData, ItemCnt);
		}
	}
}


void AQLCharacterPlayer::ServerRPCUpdateAmmo_Implementation(uint32 ItemCnt)
{
	if (GetNetMode() == ENetMode::NM_Client)
	{
		QLInventory->InventoryItem[EItemType::Ammo] = ItemCnt;
	}
}

bool AQLCharacterPlayer::GetIsJumping()
{
	return GetCharacterMovement()->IsFalling();
}


FVector AQLCharacterPlayer::GetVelocity() const
{
	FVector Velocity = Super::GetVelocity();

	float GroundSpeed = Velocity.Size2D(); //Z값을 제외한 X/Y에 대한 크기
	bool bisIdle = GroundSpeed < MovingThreshold;

	if (bisIdle&&ASC&&IsLocallyControlled())
	{
		FGameplayTagContainer TagContainer(CHARACTER_STATE_STOP);

		if (ASC->HasAnyMatchingGameplayTags(TagContainer) == false)
		{
			ASC->AddLooseGameplayTags(TagContainer);
		}
	}
	return Velocity;
}

void AQLCharacterPlayer::MulticastRPCSwitchAttackType_Implementation(ECharacterAttackType InputKey)
{
	switch (InputKey)
	{
	case ECharacterAttackType::HookAttack:
		
		ASC->AddLooseGameplayTag(CHARACTER_EQUIP_NON);
		Weapon->Weapon->SetHiddenInGame(true); //총 숨김 (애니메이션도 풀어야함) => 이친구는,,,멀티캐스트 RPC 필요
		if (Weapon->Bomb != nullptr)
		{
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

	UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());


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

	AQLPlayerController* PC = GetController<AQLPlayerController>();

	if (ASC&& NewCount == 1)
	{
		ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_GUNTYPEA);
		ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_BOMB);
		CurrentAttackType = ECharacterAttackType::HookAttack;
		if (IsLocallyControlled())
		{
			PC->SwitchWeaponStyle(CurrentAttackType);
		}
	}
}

void AQLCharacterPlayer::ResetEquipTypeA(const FGameplayTag CallbackTag, int32 NewCount)
{

	AQLPlayerController* PC = GetController<AQLPlayerController>();
	if (ASC&& NewCount == 1)
	{
		ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_NON);
		ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_BOMB);
		if (bIsSemiAutomatic)
		{
			CurrentAttackType = ECharacterAttackType::GunAttack; //만약 연사이면 연사로 변경해야함.
		}
		else
		{
			CurrentAttackType = ECharacterAttackType::AutomaticGunAttack;
		}

		if (IsLocallyControlled())
		{
			if (bHasGun == false)
			{
				PC->SwitchWeaponStyle(CurrentAttackType); //만약 연사이면 연사로 변경해야함.
			}
		}
	}
}

void AQLCharacterPlayer::ResetBomb(const FGameplayTag CallbackTag, int32 NewCount)
{

	AQLPlayerController* PC = GetController<AQLPlayerController>();
	if (ASC&& NewCount == 1)
	{
		ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_NON);
		ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_GUNTYPEA);
		CurrentAttackType = ECharacterAttackType::BombAttack;

		if (IsLocallyControlled())
		{
			if (GetInventoryCnt(EItemType::Bomb))
			{
				PC->SwitchWeaponStyle(CurrentAttackType);
			}
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
		ASC->ClearAllAbilities();

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
}

void AQLCharacterPlayer::ClientRPCPlayAnimation_Implementation(AQLCharacterPlayer* CharacterPlay)
{
	if (CharacterPlay)
	{
		//몽타주 실행, 실행중이라면 실행하지 않음.
		if (CharacterPlay->IsMontagePlaying(CharacterPlay->PickupMontage) == false)
		{
			CharacterPlay->PlayAnimMontage(CharacterPlay->PickupMontage);
		}
	}
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

bool AQLCharacterPlayer::ServerRPCPuttingWeapon_Validate()
{
	return bHasGun != false;
}

void AQLCharacterPlayer::ServerRPCPuttingWeapon_Implementation()
{
	// Multicast 위치 or Server 위치하고 Replicated할지.. 
	FVector Location = GetActorLocation();
	Location.X -= 30.0f;
	Location.Y -= 30.0f;
	FActorSpawnParameters Params;
	AQLItemBox* GroundItem = GetWorld()->SpawnActor<AQLItemBox>(Weapon->GroundWeapon, Location, FRotator::ZeroRotator, Params);
	GroundItem->SetPhysics();
	CurrentAttackType = ECharacterAttackType::HookAttack;

	UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());

	AQLPlayerState* PS = CastChecked<AQLPlayerState>(GetPlayerState());
	
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
	Weapon->Weapon->SetSkeletalMesh(nullptr);
	bHasGun = false;
}

//Server Section
void AQLCharacterPlayer::GetItem(AQLItem* ItemInfo)
{
	UQLItemData* ItemData = Cast<UQLItemData>(ItemInfo->Stat);

	int32 ItemCnt = 1;
	
	QLInventory->AddItem(ItemData->ItemType, ItemCnt); //Server Cnt Increase
	QLInventory->ClientRPCAddItem(ItemData->ItemType, ItemCnt);

	ItemInfo->SetLifeSpan(0.5f);
}

int AQLCharacterPlayer::GetInventoryCnt(EItemType ItemType)
{
	return QLInventory->GetInventoryCnt(ItemType);
}

void AQLCharacterPlayer::InitNickname()
{
	AQLPlayerState* PS = Cast<AQLPlayerState>(GetPlayerState());
	if (PS)
	{
		SetNickname(PS->GetPlayerName());
	}
}

void AQLCharacterPlayer::StopAim()
{
	if (QLInputComponent)
	{
		QLInputComponent->StopAiming();
	}
}

void AQLCharacterPlayer::ServerRPCInitNickname_Implementation()
{
	AQLPlayerState* PS = Cast<AQLPlayerState>(GetPlayerState());
	if (PS)
	{
		MulticastRPCInitNickname(PS->GetPlayerName());
	}
}

void AQLCharacterPlayer::ServerRPCDetachBomb_Implementation()
{
	
	TArray<AActor*> ChildActors;
	GetAttachedActors(ChildActors, true);

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponent();
	const UQLAS_WeaponStat* WeaponStat = SourceASC->GetSet<UQLAS_WeaponStat>();

	for (const auto& AttachedChild : ChildActors)
	{
		AQLBomb* Bomb = Cast<AQLBomb>(AttachedChild);

		if (Bomb != nullptr)
		{
			Bomb->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			FVector OutVelocity = GetCameraForward() * WeaponStat->GetAttackSpeed();
			Bomb->ThrowBomb(OutVelocity);
			break;
		}
	}
}
void AQLCharacterPlayer::OnPlayMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (IsLocallyControlled()&&NotifyName == FName(TEXT("ThrowAnimNofity")))
	{
		bThrowBomb = true;
		ServerRPCDetachBomb();
	}

	if (NotifyName == FName(TEXT("StopProneMontage")))
	{
		QL_LOG(QLLog, Log, TEXT("Stop Prone"));
		if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_PRONE))
		{
			ASC->RemoveLooseGameplayTag(CHARACTER_STATE_PRONE);
		}
		//StopProneMontage가 알림오면, 움직일 수 있도록한다.
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}

	if (NotifyName == FName(TEXT("StartProneMontage")))
	{
		if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_PRONE) == false)
		{
			ASC->AddLooseGameplayTag(CHARACTER_STATE_PRONE);
		}

		if (bIsProning) 
		{
			//엎드리는 중
			StandToProne();
		}
		else 
		{
			//일어나는 중
			ProneToStand();
		}
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		//걷지 못하도록 MovementMode 변경
	}
}

void AQLCharacterPlayer::OnPlayMontageNotifyEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage->GetName() == TEXT("M_Prone_To_Stand"))
	{
		if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_PRONE))
		{
			ASC->RemoveLooseGameplayTag(CHARACTER_STATE_PRONE);
		}
		//StopProneMontage가 알림오면, 움직일 수 있도록한다.
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	
	if (Montage->GetName() == TEXT("M_Stand_To_Prone"))
	{
		if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_PRONE))
		{
			ASC->RemoveLooseGameplayTag(CHARACTER_STATE_PRONE);
		}
		//StopProneMontage가 알림오면, 움직일 수 있도록한다.
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}

void AQLCharacterPlayer::StartHorizontalRecoil(float Value)
{
	AddControllerYawInput(Value);
}

void AQLCharacterPlayer::StartVerticalRecoil(float Value)
{
	AddControllerPitchInput(Value); //캐릭터에 반동을 제공한다.
}

void AQLCharacterPlayer::StartRecoil()
{
	RecoilTimeline.PlayFromStart();
}

void AQLCharacterPlayer::ReverseRecoil()
{
	RecoilTimeline.Reverse();
}

void AQLCharacterPlayer::ProneToStand()
{
	UQLCharacterMovementComponent* Movement = Cast<UQLCharacterMovementComponent>(GetMovementComponent());

	FVector CurrentLocation = GetActorLocation(); //현재 Pawn 위치
	CurrentLocation.Z = StartHeight; //Pawn 높이값
	SetActorLocation(CurrentLocation); //위치를 맞춰준다.
	if (IsLocallyControlled()) 
	{
		//클라이언트의 경우 Mesh의 중심위치를 조절한다.
		GetMesh()->SetRelativeLocation(StandMeshLoc); //약 FVector(0.f, 0.f, -100.f)
	}
	else
	{
		//서버의 경우, 클라이언트에 맞춰진 크기로 메쉬 중심지를 맞춘다.
		CacheInitialMeshOffset(StandMeshLoc, GetMesh()->GetRelativeRotation());
	}

	GetCapsuleComponent()->SetCapsuleSize(OriginalCapsuleRadius, OriginalCapsuleHeight); //OriginalCapsuleSize로 변경, 25.f, 90.0f
	Movement->RestoreProneSpeedCommand();
}

void AQLCharacterPlayer::StandToProne()
{
	UQLCharacterMovementComponent* Movement = Cast<UQLCharacterMovementComponent>(GetMovementComponent());

	FVector TargetLocation = GetMesh()->GetSocketLocation(FName("ik_foot_rootSocket"));
	SetActorLocation(TargetLocation); //발 중심지를 위치로 지정
	if (IsLocallyControlled())
	{
		//클라이언트의 상대값 변경
		GetMesh()->SetRelativeLocation(ProneMeshLoc); //ProneMeshLoc FVector(0.0f, 0.f, -30.0f)
	}
	else
	{
		//변경된 상대값을 옵셋 설정
		CacheInitialMeshOffset(ProneMeshLoc, GetMesh()->GetRelativeRotation());
	}
	//캡슐 사이즈 지정 OriginalCapsuleRadius 25 ProneCapsuleHeight 25
	GetCapsuleComponent()->SetCapsuleSize(OriginalCapsuleRadius, ProneCapsuleHeight);
	Movement->ChangeProneSpeedCommand(); //Prone 속도 
}
