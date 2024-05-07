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
		bIsAlreadyHidden = false;
		OnLifespanDelegate = nullptr;
		OnLifestoneChangedDelegate = nullptr; //Lifestone을 가져갔음, 즉 역할이 사라짐.
		PlayerStateName = TEXT("");
	}
	else
	{
		bIsAlreadyHidden = true;
		PlayerStateName = InPlayerStateName;

		//숨길 때 델리게이트를 사용해서 가져갔음을 전달. -> PlayerState 
		if (OnLifestoneChangedDelegate.IsBound())
		{
			OnLifestoneChangedDelegate.Execute(false);
		}
		QL_LOG(QLLog, Warning, TEXT("Hidden %s"), *PlayerStateName.ToString());
	}
}
