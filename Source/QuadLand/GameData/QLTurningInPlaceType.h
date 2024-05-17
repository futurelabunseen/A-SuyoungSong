#pragma once
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

//회전에 대한 정의를 Enum클래스에서 수행
UENUM(BlueprintType)
enum class ETurningPlaceType : uint8
{
	ETIP_Left UMETA(DisplayName = "Turning Left"),
	ETIP_Right UMETA(DisplayName = "Turning Right"),
	ETIP_NotTurning UMETA(DisplayName = "Not Turning"),
	ETIP_Max UMETA(DisplayName = "Default Max")
};
