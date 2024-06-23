// Fill out your copyright notice in the Description page of Project Settings.


#include "QLItemBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameData/QLWeaponStat.h"
#include "Item/QLWeaponItemBox.h"
#include "Physics/QLCollision.h"
#include "UI/QLAlertPanel.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "QuadLand.h"

// Sets default values
AQLItemBox::AQLItemBox()
{

	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));

	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);

	Trigger->SetCollisionProfileName(CPROFILE_QLTRIGGER);
	Trigger->SetBoxExtent(FVector(50.0f, 5.0f, 5.0f));
	Mesh->SetRelativeLocationAndRotation(FVector(-20.0f, -10.0f, 0.f), FRotator(90.f, -90.f, 0.f)); //Mesh 기준이기 때문에 아이템에서는 변경 예정
	Mesh->SetCollisionProfileName(CPROFILE_QLPHYSICS);

	bReplicates = true;
	SetReplicateMovement(true);

	AlertComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Farming"));
	AlertComponent->SetupAttachment(Trigger);
	AlertComponent->AttachToComponent(Trigger, FAttachmentTransformRules::KeepRelativeTransform, TEXT("head"));
	AlertComponent->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
	FVector RelativeLocation(40.f, 0.f, 100.f);
	FRotator RelativeRotation(0.f, 0.0f, -180.0f);

	AlertComponent->SetRelativeLocation(RelativeLocation);
	AlertComponent->SetRelativeRotation(RelativeRotation);

	static ConstructorHelpers::FClassFinder<UUserWidget> FarmingWidgetRef(TEXT("/Game/QuadLand/UI/WBP_Farming.WBP_Farming_C"));

	if (FarmingWidgetRef.Class)
	{
		AlertComponent->SetWidgetSpace(EWidgetSpace::World); //2D변경
		AlertComponent->SetDrawSize(FVector2D(1500.0f, 300.0f));
		AlertComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AlertComponent->SetWidgetClass(FarmingWidgetRef.Class);
	}

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AQLItemBox::OnOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AQLItemBox::OnEndOverlap);

	NetCullDistanceSquared = 4000000.0f;
	AlertComponent->SetHiddenInGame(true);
}


void AQLItemBox::InitPosition(const FVector& Location)
{
	if (HasAuthority())
	{
		FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(GroundCheckLineTrace), false, this); //식별자 

		FHitResult OutHitResult;

		FVector StartLocation = Location;

		FVector EndLocation = StartLocation + 150.0f * GetActorUpVector() * -1;
		bool bResult = GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			StartLocation,
			EndLocation,
			CCHANNEL_QLGROUND,
			CollisionParams
		);

		if (bResult)
		{
			OutHitResult.Location.Z += GetZPos();
			SetActorLocation(OutHitResult.Location);
		}

		AQLWeaponItemBox* WeaponBox = Cast<AQLWeaponItemBox>(this);

		if (WeaponBox)
		{
			WeaponBox->SpawnBulletsAround();
		}
	}
}

void AQLItemBox::SetPhysics()
{
	FCollisionQueryParams CollisionParams(SCENE_QUERY_STAT(GroundCheckLineTrace), false, this); //식별자 

	FHitResult OutHitResult;

	FVector StartLocation = GetActorLocation();

	FVector EndLocation = StartLocation + 150.0f * GetActorUpVector() * -1;
	bool bResult = GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		StartLocation,
		EndLocation,
		CCHANNEL_QLGROUND,
		CollisionParams
	);

	if (bResult)
	{
		FVector Location = OutHitResult.Location;
		Location.Z += GetZPos();
		SetActorLocation(Location);
	}
}

float AQLItemBox::GetZPos()
{
	return Trigger->GetScaledBoxExtent().Z / 2.0f;
}

void AQLItemBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation(); //현재 자신의 카메라의 위치 
		FVector CurLoc = AlertComponent->GetComponentLocation();
		FRotator Rot = UKismetMathLibrary::FindLookAtRotation(CurLoc, CameraLocation);

		AlertComponent->SetWorldRotation(Rot);
	}
}

void AQLItemBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	AlertComponent->SetHiddenInGame(false);
}

void AQLItemBox::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AlertComponent->SetHiddenInGame(true);
}
