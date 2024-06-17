// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/QLDamageWidgetActor.h"
#include "UI/QLAttackDamageWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

AQLDamageWidgetActor::AQLDamageWidgetActor()
{
	DamageComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Damage"));
	static ConstructorHelpers::FClassFinder<UQLAttackDamageWidget> DamageRef(TEXT("/Game/QuadLand/UI/WBQL_Damage.WBQL_Damage_C"));

	FVector RelativeLocation(40.f, 0.f, 0.f);
	FRotator RelativeRotation(0.f, -90.f, 0.f);

	DamageComponent->SetRelativeLocation(RelativeLocation);
	DamageComponent->SetRelativeRotation(RelativeRotation);

	if (DamageRef.Class)
	{
		DamageComponent->SetWidgetSpace(EWidgetSpace::Screen); //2Dº¯°æ
		DamageComponent->SetDrawSize(FVector2D(250.0f, 250.0f));
		DamageComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DamageComponent->SetWidgetClass(DamageRef.Class);
	}

	bReplicates = true;
}

void AQLDamageWidgetActor::SetDamage(const float &InDamage)
{
	MulitcastRPCShowDamage(InDamage);

}

void AQLDamageWidgetActor::MulitcastRPCShowDamage_Implementation(float InDamage)
{
	UQLAttackDamageWidget* Widget = Cast<UQLAttackDamageWidget>(DamageComponent->GetWidget());

	if (Widget)
	{
		Widget->SetDamage(InDamage);
	}
}

