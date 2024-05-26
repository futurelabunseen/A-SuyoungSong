// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/QLWeaponItemBox.h"
#include "QuadLand.h"

void AQLWeaponItemBox::SpawnBulletsAround()
{
	if (AmmoClass)
	{
		FVector Location = GetActorLocation(); //Possessed Pawn Position
		Location.Z = 100.0f;
		FActorSpawnParameters Params;
		Params.Owner = this;
		
		AQLItemBox* ItemBox = GetWorld()->SpawnActor<AQLItemBox>(AmmoClass, Location, FRotator::ZeroRotator, Params);
		ItemBox->InitPosition();
	}
}
