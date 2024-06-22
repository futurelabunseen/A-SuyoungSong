// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/QLLifestoneStorageBox.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Physics/QLCollision.h"
#include "Components/WidgetComponent.h"
#include "UI/QLAlertPanel.h"
#include "Interface/QLLifestoneContainerInterface.h"
#include "QuadLand.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AQLLifestoneStorageBox::AQLLifestoneStorageBox()
{

	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemBox"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Script/Engine.StaticMesh'/Game/VikingVillage/StaticMeshes/SM_Barrel.SM_Barrel'"));

	if (MeshRef.Object)
	{
		Mesh->SetStaticMesh(MeshRef.Object);
	}

	RootComponent = Trigger;
	
	Trigger->SetCollisionProfileName(CPROFILE_QLTRIGGER);
	Trigger->SetSphereRadius(150.0f);

	Mesh->SetupAttachment(Trigger);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AQLLifestoneStorageBox::OnComponentBeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AQLLifestoneStorageBox::OnComponentEndOverlap);

	PlayerStateName = TEXT("");
	bIsAlreadyHidden = false;

	AlertComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("AlertPanel"));
	AlertComponent->SetupAttachment(Mesh);
	AlertComponent->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("head"));

	FVector RelativeLocation(0.f, 0.f, 150.f);

	AlertComponent->SetRelativeLocation(RelativeLocation);
	//AlertComponent->SetRelativeRotation(RelativeRotation);

	static ConstructorHelpers::FClassFinder<UQLAlertPanel> AlertPanelClassRef(TEXT("/Game/QuadLand/UI/WBQL_AlertPanel.WBQL_AlertPanel_C"));

	if (AlertPanelClassRef.Class)
	{
		AlertComponent->SetWidgetSpace(EWidgetSpace::World); //2D변경
		AlertComponent->SetDrawSize(FVector2D(250.0f, 60.0f));
		AlertComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AlertComponent->SetWidgetClass(AlertPanelClassRef.Class);
	}
	AlertComponent->SetHiddenInGame(true);
}

void AQLLifestoneStorageBox::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	RotateStorageBox();
}

void AQLLifestoneStorageBox::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character == nullptr)
	{
		return; 
	}

	AlertComponent->SetHiddenInGame(false);
	IQLLifestoneContainerInterface* LifeStoneInterface = Cast<IQLLifestoneContainerInterface>(OtherActor);
	
	if (LifeStoneInterface)
	{
		LifeStoneInterface->CheckBoxOverlap();
	}
}

void AQLLifestoneStorageBox::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AlertComponent->SetHiddenInGame(true);

	IQLLifestoneContainerInterface* LifeStoneInterface = Cast<IQLLifestoneContainerInterface>(OtherActor);
	if (LifeStoneInterface)
	{
		LifeStoneInterface->CheckBoxOverlap();
	}
}
void AQLLifestoneStorageBox::RotateStorageBox()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0); //첫번째 플레이어 컨트롤러를 가져옴, 즉 원격이든 어디든 0번째
	if (!PlayerController || !PlayerController->PlayerCameraManager)
	{
		return;
	}

	FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation(); //현재 자신의 카메라의 위치 
	FVector ActorLocation = GetActorLocation(); // 액터의 이동 위치

	FRotator LookAtRotation = (CameraLocation - ActorLocation).Rotation(); //둘이 바라보는 위치를 빼면, Rotate 값이 나오고, 위젯 자체가 회전하는 중임.
	LookAtRotation.Pitch = 0;  // Keep the widget upright

	AlertComponent->SetWorldRotation(LookAtRotation);
}

void AQLLifestoneStorageBox::UpdateAlertPanel(FName InPlayerStateName)
{
	UQLAlertPanel* Panel = Cast<UQLAlertPanel>(AlertComponent->GetWidget());
	PlayerStateName = InPlayerStateName;

	if (Panel)
	{
		FString Nickname = PlayerStateName.ToString();
		Panel->SetNickNameTxt(Nickname);

		if (Nickname == TEXT("None"))
		{
			bIsAlreadyHidden = false;
			Panel->SetStatusTxt(TEXT("Empty"));
		}
		else
		{
			bIsAlreadyHidden = true;
			Panel->SetStatusTxt(TEXT("Full"));
		}
	}
}

void AQLLifestoneStorageBox::ConcealLifeStone(FName InPlayerStateName,bool HasLifeStone)
{
	if (bIsAlreadyHidden)
	{
		if (InPlayerStateName != TEXT("") && InPlayerStateName != PlayerStateName)
		{
			//죽음 태그 부착한다. -> 태그를 부착하고 Dead 어빌리티에서 10초가량 시간을 제공해주는 걸로 변경
			if (OnLifespanDelegate.IsBound())
			{
				OnLifespanDelegate.Execute();
			}
		}
		else {
			
			//자기 자신이 가져가는 것을 의미하기 때문에 -> 리셋처리한 후 PlayerState bHasLifestone =true처리가 된다.
			if (OnLifestoneChangedDelegate.IsBound())
			{
				OnLifestoneChangedDelegate.Execute(true);
				QL_LOG(QLNetLog, Log, TEXT("come on lifestone"));
			}
		}
		
		OnLifespanDelegate = nullptr;
		OnLifestoneChangedDelegate = nullptr; //Lifestone을 가져갔음, 즉 역할이 사라짐.
		PlayerStateName = TEXT("");
	}
	else
	{
		if (HasLifeStone)
		{
			PlayerStateName = InPlayerStateName;
		}
		//숨길 때 델리게이트를 사용해서 가져갔음을 전달. -> PlayerState 
		if (OnLifestoneChangedDelegate.IsBound())
		{
			OnLifestoneChangedDelegate.Execute(false);
		}
		
		
	}

	if (OnUpdateAlertPanel.IsBound())
	{
		OnUpdateAlertPanel.Execute(PlayerStateName,this);
	}
}