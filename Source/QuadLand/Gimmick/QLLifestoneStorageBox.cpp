// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/QLLifestoneStorageBox.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Physics/QLCollision.h"
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
}

void AQLLifestoneStorageBox::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	IQLLifestoneContainerInterface* LifeStoneInterface = Cast<IQLLifestoneContainerInterface>(OtherActor);
	QL_LOG(QLNetLog, Log, TEXT("Begin"));

	if (LifeStoneInterface)
	{
		LifeStoneInterface->CheckBoxOverlap();
	}
}

void AQLLifestoneStorageBox::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	QL_LOG(QLNetLog, Log, TEXT("End"));
	IQLLifestoneContainerInterface* LifeStoneInterface = Cast<IQLLifestoneContainerInterface>(OtherActor);
	if (LifeStoneInterface)
	{
		LifeStoneInterface->CheckBoxOverlap();
	}
}

void AQLLifestoneStorageBox::ConcealLifeStone(FName InPlayerStateName)
{
	if (bIsAlreadyHidden)
	{
		if (InPlayerStateName != TEXT("") && InPlayerStateName != PlayerStateName)
		{
			QL_LOG(QLLog, Warning, TEXT("%s %s"), *PlayerStateName.ToString(), *InPlayerStateName.ToString());
			QL_LOG(QLNetLog, Log, TEXT("to be confiscated"));
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
		bIsAlreadyHidden = false;
		OnLifespanDelegate = nullptr;
		OnLifestoneChangedDelegate = nullptr; //Lifestone�� ��������, �� ������ �����.
		PlayerStateName = TEXT("");
	}
	else
	{
		bIsAlreadyHidden = true;
		PlayerStateName = InPlayerStateName;

		//���� �� ��������Ʈ�� ����ؼ� ���������� ����. -> PlayerState 
		if (OnLifestoneChangedDelegate.IsBound())
		{
			OnLifestoneChangedDelegate.Execute(false);
		}
		QL_LOG(QLLog, Warning, TEXT("Hidden %s"), *PlayerStateName.ToString());
	}
}
