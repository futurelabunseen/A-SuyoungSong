// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/QLLifestoneStorageBox.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Physics/QLCollision.h"
#include "Components/WidgetComponent.h"
#include "UI/QLAlertPanel.h"
#include "Interface/QLLifestoneContainerInterface.h"
#include "QuadLand.h"

// Sets default values
AQLLifestoneStorageBox::AQLLifestoneStorageBox()
{
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
	AlertComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
	static ConstructorHelpers::FClassFinder<UQLAlertPanel> AlertPanelClassRef(TEXT("/Game/QuadLand/UI/WBQL_AlertPanel.WBQL_AlertPanel_C"));

	if (AlertPanelClassRef.Class)
	{
		AlertComponent->SetWidgetSpace(EWidgetSpace::Screen); //2D����
		AlertComponent->SetDrawSize(FVector2D(250.0f, 60.0f));
		AlertComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AlertComponent->SetWidgetClass(AlertPanelClassRef.Class);
	}
	AlertComponent->SetHiddenInGame(true);
}

void AQLLifestoneStorageBox::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
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

void AQLLifestoneStorageBox::ConcealLifeStone(FName InPlayerStateName)
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
		PlayerStateName = InPlayerStateName;

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