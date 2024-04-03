// Fill out your copyright notice in the Description page of Project Settings.

#include "GA/TA/QLTA_LineTraceResult.h"
#include "Character/QLCharacterPlayer.h"
#include "Physics/QLCollision.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"

FGameplayAbilityTargetDataHandle AQLTA_LineTraceResult::MakeTargetData() const
{
	//Super은 안해도된다. 대체할 예정 
	AQLCharacterPlayer* Character = CastChecked<AQLCharacterPlayer>(SourceActor);

	FVector CameraStartLocation = Character->CalPlayerLocalCameraStartPos();

	float Dist = 1000.0f;
	FVector EndLocation = CameraStartLocation + Character->GetCameraForward() * Dist; //현재는 임시값 어트리뷰트 셋에서 가져올 예정
	FCollisionQueryParams Params(SCENE_QUERY_STAT(LineTraceResult), false, Character); //식별자 

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
