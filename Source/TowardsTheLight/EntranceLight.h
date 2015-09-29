// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "EntranceLight.generated.h"

UCLASS()
class TOWARDSTHELIGHT_API AEntranceLight : public AActor
{
	GENERATED_BODY()
	
public:
  //properties
  UPROPERTY(EditAnywhere, Category = EntranceTowerLight)
    UStaticMeshComponent* LightMesh;
  UPROPERTY(EditAnywhere, Category = EntranceTowerLight)
    FLinearColor ColorKey;
  //functions
	AEntranceLight();
	virtual void BeginPlay() override;
  void EndPlay(const EEndPlayReason::Type EndPlayReason);
private:
  UMaterialInstanceDynamic *TowerLightMaterial;
	
};
