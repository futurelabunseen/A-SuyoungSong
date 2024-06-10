// Fill out your copyright notice in the Description page of Project Settings.

#include "GA/TA/QLTA_LineTraceResult.h"
#include "Character/QLCharacterBase.h"
#include "Character/QLCharacterPlayer.h"
#include "Interface/QLAIAttackInterface.h"
#include "Physics/QLCollision.h"
#include "Camera/CameraComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "Item/QLWeaponComponent.h"
#include "DrawDebugHelpers.h"
#include "QuadLand.h"

FGameplayAbilityTargetDataHandle AQLTA_LineTraceResult::MakeTargetData() const
{
	FGameplayAbilityTargetDataHandle DataHandle;
	//Super은 안해도된다. 대체할 예정 
	AQLCharacterBase* Character = CastChecked<AQLCharacterBase>(SourceActor);
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(Character);

	if (ASC == nullptr)
	{
		return DataHandle;
	}
	const UQLAS_WeaponStat* WeaponStat = ASC->GetSet<UQLAS_WeaponStat>();
	float Dist = WeaponStat->GetAttackDistance();
	FVector AttackStartLocation;
	FVector AttackEndLocation;
	if (Player)
	{
		AttackStartLocation = Player->CalPlayerLocalCameraStartPos();
		AttackEndLocation = AttackStartLocation + Player->GetCameraForward() * Dist; //현재는 임시값 어트리뷰트 셋에서 가져올 예정
	}
	else
	{
		IQLAIAttackInterface* AI = Cast<IQLAIAttackInterface>(Character->GetController());
		if (AI)
		{
			const APawn* Target = AI->GetTarget();
			if (Target)
			{
				AttackStartLocation = Character->GetWeapon()->GetWeaponMesh()->GetSocketLocation(FName("MuzzleFlash"));
				FVector Location = Target->GetActorLocation();
				Location.Y += AI->GetTargetPos().X;
				Location.Z += AI->GetTargetPos().Y;

				AttackEndLocation = Location;
			}
		}
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(LineTraceResult), false, Character); //식별자 

	FHitResult OutHitResult;

	bool bResult = GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		AttackStartLocation,
		AttackEndLocation,
		CCHANNEL_QLACTION,
		Params
	);

	if (bResult)
	{
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(OutHitResult);
		DataHandle.Add(TargetData);
	}

	return DataHandle;

}
