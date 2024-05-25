// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/QLWeaponItemBox.h"
#include "QuadLand.h"

void AQLWeaponItemBox::OverlapCheck()
{

	if (AmmoClass)
	{
		FVector Location = GetActorLocation();
		FActorSpawnParameters Params;
	
		AQLItemBox *ItemBox = GetWorld()->SpawnActor<AQLItemBox>(AmmoClass, Location, FRotator::ZeroRotator, Params);
		//ItemBox->Mesh->AddImpulse()
	}
}
