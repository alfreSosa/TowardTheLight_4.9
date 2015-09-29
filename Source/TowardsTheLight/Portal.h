// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tappable.h"
#include "Portal.generated.h"

/**
 * 
 */
UCLASS()
class TOWARDSTHELIGHT_API APortal : public ATappable
{
	GENERATED_BODY()
	
public:
  //constructor
  APortal();

  //Properties
  UPROPERTY(EditAnywhere)
    UStaticMeshComponent* MeshBG;
  UPROPERTY(EditAnywhere)
    UStaticMeshComponent* MeshEffects;

  UPROPERTY(EditAnywhere, Category = Portal)
    APortal *Partner;
  UPROPERTY(EditAnywhere, Category = Portal)
    FLinearColor ColorDisabled;
  UPROPERTY(EditAnywhere, Category = Portal)
    FLinearColor ColorEnabled;
  UPROPERTY(EditAnywhere, Category = Portal)
    FLinearColor PortalColor;

  //Class Functions
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaSeconds) override;
  void EndPlay(const EEndPlayReason::Type EndPlayReason);
  virtual void Activate(bool enabled);
  virtual void Execute();
  void Turn(bool on);

private:
  UMaterialInstanceDynamic *PortalMaterial;
  UMaterialInstanceDynamic *PortalMaterialBG;
  UMaterialInstanceDynamic *PortalMaterialEffects;
  bool m_activateTranslate;
  float m_elapsedTranslate;
  bool m_isRunning;
};
