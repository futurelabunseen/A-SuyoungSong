// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/QLUIType.h"
#include "QLHUD.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLHUD : public AHUD
{
	GENERATED_BODY()
public:

	void CreateHUD();
	virtual void BeginPlay() override;

	/*HUD �� ���� ��*/
	UFUNCTION(BlueprintCallable)
	void SetVisibilityHUD(EHUDType UItype);
	/*HUD �� �ݴ� ��*/
	UFUNCTION(BlueprintCallable)
	void SetHiddenHUD(EHUDType UItype);
	
	//���� HUD�� ����
	int HUDNum() {
		return HUDs.Num();
	}

	/*PlayerController���� �ε��� �ʸ� ������Ʈ�ϴ� �뵵*/
	void UpdateLoadingSec(float Time);
protected:
	/*���� ���� HUD ����*/
	void InitStoneTexture(int GemType);
	/*�г��� ���� HUD ����*/
	void SettingNickname();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TMap<EHUDType, TObjectPtr<class UUserWidget>> HUDs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TMap<EHUDType, TSubclassOf<class UUserWidget>> HUDClass;

protected:
	/*2�� �̻� ����ϴ� PlayerState �� �̸� ������ ����.*/
	UPROPERTY()
	TObjectPtr<class AQLPlayerState> LocalPlayerState;

};
