// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLAttackDamageComponent.h"
#include "UI/QLAttackDamageWidget.h"
UQLAttackDamageComponent::UQLAttackDamageComponent()
{

	static ConstructorHelpers::FClassFinder<UQLAttackDamageWidget> DamageRef(TEXT("/Game/QuadLand/UI/WBQL_Damage.WBQL_Damage_C"));

	if (DamageRef.Class)
	{
		SetWidgetSpace(EWidgetSpace::World); //2Dº¯°æ
		SetDrawSize(FVector2D(250.0f, 30.0f));
		SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetWidgetClass(DamageRef.Class);
	}
}
