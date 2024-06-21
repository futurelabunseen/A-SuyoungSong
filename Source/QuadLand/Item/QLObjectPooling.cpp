// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/QLObjectPooling.h"
#include "QuadLand.h"

UQLObjectPooling::UQLObjectPooling()
{

}

void UQLObjectPooling::Init(AActor* InActor)
{
	//Collision ����
	//���� ���� �����
	InActor->SetActorEnableCollision(false);
	InActor->SetHidden(false);
	InActor->SetActorLocation(GetOwner()->GetActorLocation());
}

AActor* UQLObjectPooling::GetObject()
{
	AActor* FirstBullet = nullptr;

	QL_SUBLOG(QLLog, Log, TEXT("PoolManager %d"), PoolManager.Num());
	if (PoolManager.Num() != 0)
	{	//������.
		FirstBullet = PoolManager.Pop().Get();
		FirstBullet->SetActorEnableCollision(true);
		FirstBullet->SetHidden(false);
	}
	return FirstBullet;
}

void UQLObjectPooling::PutBackToPool(AActor* InActor)
{
	Init(InActor);
	PoolManager.Add(InActor);
}
