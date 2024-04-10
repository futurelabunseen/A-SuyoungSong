// Fill out your copyright notice in the Description page of Project Settings.

#include "GA/TA/QLTA_LineTraceResult.h"
#include "Character/QLCharacterPlayer.h"
#include "Physics/QLCollision.h"
#include "Camera/CameraComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AttributeSet/QLAS_WeaponStat.h"

#include "DrawDebugHelpers.h"
#include "QuadLand.h"

FGameplayAbilityTargetDataHandle AQLTA_LineTraceResult::MakeTargetData() const
{
	//Super�� ���ص��ȴ�. ��ü�� ���� 
	AQLCharacterPlayer* Character = CastChecked<AQLCharacterPlayer>(SourceActor);

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);
	FVector CameraStartLocation = Character->CalPlayerLocalCameraStartPos();

	float Dist = 1000.0f;
	if (ASC != nullptr)
	{
		const UQLAS_WeaponStat* WeaponStat = ASC->GetSet<UQLAS_WeaponStat>();

		Dist = WeaponStat->GetAttackDistance();
		
		UE_LOG(LogTemp, Log, TEXT("changed distance %lf"), Dist);
	}
	FVector EndLocation = CameraStartLocation + Character->GetCameraForward() * Dist; //����� �ӽð� ��Ʈ����Ʈ �¿��� ������ ����
	FCollisionQueryParams Params(SCENE_QUERY_STAT(LineTraceResult), false, Character); //�ĺ��� 

	FHitResult OutHitResult;

	bool bResult = GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		CameraStartLocation,
		EndLocation,
		CCHANNEL_QLACTION,
		Params
	);

	FGameplayAbilityTargetDataHandle DataHandle;
	if (bResult)
	{
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(OutHitResult);
		DataHandle.Add(TargetData);
	}

#if ENABLE_DRAW_DEBUG
	FColor Color = bResult ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), CameraStartLocation, EndLocation, Color, false, 5.0f);
#endif

	return DataHandle;
}
