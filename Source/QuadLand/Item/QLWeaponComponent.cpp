// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/QLWeaponComponent.h"
#include "GameFramework/Character.h"
#include "GameData/QLDataManager.h"
#include "GameData/QLBombStat.h"
#include "Item/QLBomb.h"
UQLWeaponComponent::UQLWeaponComponent()
{
	//GetOwner()
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));

	//AnimInstance
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceRef(TEXT("/Game/QuadLand/Animations/Blueprint/ABQL_WeaponTypeA.ABQL_WeaponTypeA_C"));

	if (AnimInstanceRef.Class)
	{
		Weapon->SetAnimClass(AnimInstanceRef.Class);
	}

}

void UQLWeaponComponent::SpawnBomb()
{
	if (Bomb != nullptr)
	{
		return;
	}

	UQLDataManager* DataManager = GetWorld()->GetSubsystem<UQLDataManager>();
	UQLBombStat* Stat = Cast<UQLBombStat>(DataManager->GetItem(EItemType::Bomb));
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (DataManager&& Character)
	{
		//나머지 클라이언트 생성
		FActorSpawnParameters Params;
		Params.Owner = Character;
		//Bomb - Actor 생성
		//클라이언트, 서버 생성 
		Bomb = GetWorld()->SpawnActor<AQLBomb>(Stat->BombClass, Params);
		if (Bomb)
		{
			OnDestoryBomb.BindUObject(this, &UQLWeaponComponent::ResetBomb);
		}
		Bomb->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("Bomb"));
	}
}

void UQLWeaponComponent::SetBombHiddenInGame(bool InHiddenInGame)
{
	if (Bomb)
	{
		Bomb->SetActorHiddenInGame(InHiddenInGame);
	}
}

void UQLWeaponComponent::ResetBomb()
{
	Bomb = nullptr;
}
