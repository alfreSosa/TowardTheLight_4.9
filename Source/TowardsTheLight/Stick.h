// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Stick.generated.h"

UCLASS()
class TOWARDSTHELIGHT_API AStick : public AActor
{
	GENERATED_BODY()
	
public:	

  UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Stick;

	//UE4 Functions
	AStick();
	virtual void BeginPlay() override;
  void EndPlay(const EEndPlayReason::Type EndPlayReason);

  //Own functions
  void SetColor(FLinearColor colorKey, float shinnes);
private:
  UMaterialInstanceDynamic *StickMaterial;
  UMaterialInstanceDynamic *EffectsMaterial;
  UMaterialBillboardComponent *BBMaterial;
	
};
