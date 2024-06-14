// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/QLDamageWidgetActor.h"
#include "UI/QLAttackDamageComponent.h"
AQLDamageWidgetActor::AQLDamageWidgetActor()
{
	DamageWidgetComponent = CreateDefaultSubobject<UQLAttackDamageComponent>(TEXT("Damage"));

	RootComponent = DamageWidgetComponent;

	FVector RelativeLocation(0.f, 0.f, 0.f);
	FRotator RelativeRotation(0.f, 0.f, -90.f);

	DamageWidgetComponent->SetRelativeLocation(RelativeLocation);
	DamageWidgetComponent->SetRelativeRotation(RelativeRotation);

	bReplicates = true;
}

