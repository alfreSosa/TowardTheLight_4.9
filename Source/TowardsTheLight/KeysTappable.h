// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "KeysTappable.generated.h"

UCLASS()
class TOWARDSTHELIGHT_API AKeysTappable : public AActor
{
	GENERATED_BODY()
	
public:	
  //properties
  UPROPERTY(EditAnywhere, Category = TappableColorKeyLight)
    UStaticMeshComponent* KeyMesh;
  UPROPERTY(EditAnywhere, Category = TappableColorKeyLight)
    FLinearColor ColorKey;
  //functions
  AKeysTappable();
	virtual void BeginPlay() override;
  void EndPlay(const EEndPlayReason::Type EndPlayReason);

private:
  UMaterialInstanceDynamic *TowerLightMaterial;
	
};
