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
		AlertComponent->SetWidgetSpace(EWidgetSpace::World); //2D����
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
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0); //ù��° �÷��̾� ��Ʈ�ѷ��� ������, �� �����̵� ���� 0��°
	if (!PlayerController || !PlayerController->PlayerCameraManager)
	{
		return;
	}

	FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation(); //���� �ڽ��� ī�޶��� ��ġ 
	FVector ActorLocation = GetActorLocation(); // ������ �̵� ��ġ

	FRotator LookAtRotation = (CameraLocation - ActorLocation).Rotation(); //���� �ٶ󺸴� ��ġ�� ����, Rotate ���� ������, ���� ��ü�� ȸ���ϴ� ����.
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
			//���� �±� �����Ѵ�. -> �±׸� �����ϰ� Dead �����Ƽ���� 10�ʰ��� �ð��� �������ִ� �ɷ� ����
			if (OnLifespanDelegate.IsBound())
			{
				OnLifespanDelegate.Execute();
			}
		}
		else {
			
			//�ڱ� �ڽ��� �������� ���� �ǹ��ϱ� ������ -> ����ó���� �� PlayerState bHasLifestone =trueó���� �ȴ�.
			if (OnLifestoneChangedDelegate.IsBound())
			{
				OnLifestoneChangedDelegate.Execute(true);
				QL_LOG(QLNetLog, Log, TEXT("come on lifestone"));
			}
		}
		
		OnLifespanDelegate = nullptr;
		OnLifestoneChangedDelegate = nullptr; //Lifestone�� ��������, �� ������ �����.
		PlayerStateName = TEXT("");
	}
	else
	{
		if (HasLifeStone)
		{
			PlayerStateName = InPlayerStateName;
		}
		//���� �� ��������Ʈ�� ����ؼ� ���������� ����. -> PlayerState 
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