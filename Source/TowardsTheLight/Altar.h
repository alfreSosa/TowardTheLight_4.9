// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tappable.h"
#include "Altar.generated.h"

UCLASS()
class TOWARDSTHELIGHT_API AAltar : public ATappable
{
	GENERATED_BODY()
public:	
  UPROPERTY(EditAnywhere, Category = Altar)
    bool GiveKey;
  UPROPERTY(EditAnywhere, Category = Altar)
    FLinearColor AltarColor;
  UPROPERTY(EditAnywhere, Category = Altar)
    FLinearColor DisableColor;

  AAltar();
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaSeconds) override;
  void EndPlay(const EEndPlayReason::Type EndPlayReason);
  virtual void Activate(bool enabled);
  virtual void Execute();
private:
  UMaterialInstanceDynamic *AltarMaterial;
};
