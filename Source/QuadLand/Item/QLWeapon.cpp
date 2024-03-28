// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/QLWeapon.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Physics/QLCollision.h"
#include "Interface/ItemFarmingInterface.h"
// Sets default values
AQLWeapon::AQLWeapon()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);

	Trigger->SetCollisionProfileName(CPROFILE_QLTRIGGER);
	Trigger->SetBoxExtent(FVector(50.0f, 5.0f, 5.0f));


	Mesh->SetRelativeLocationAndRotation(FVector(-20.0f, -10.0f, 0.f),FRotator(90.f,0.f,0.f));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	Trigger->OnComponentEndOverlap.AddDynamic(this, &AQLWeapon::OnOverlapEnd);
}

void AQLWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	
	IItemFarmingInterface* ItemPickupActor = Cast<IItemFarmingInterface>(OtherComp);

	if (ItemPickupActor)
	{
		ItemPickupActor->PickupItem();
	}

}

void AQLWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	UE_LOG(LogTemp, Log, TEXT("?????"));
}

