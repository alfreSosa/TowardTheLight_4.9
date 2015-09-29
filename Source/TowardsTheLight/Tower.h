// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Tower.generated.h"

UCLASS()
class TOWARDSTHELIGHT_API ATower : public APawn
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ATower();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
  virtual void Tick(float DeltaSeconds) override;
  UPROPERTY(EditAnywhere)
    UBoxComponent *Trigger;
  UPROPERTY(EditAnywhere, Category = Tower)
    UStaticMeshComponent* Body;
  UPROPERTY(EditAnywhere, Category = Tower)
    UStaticMeshComponent* Entrance;
  UPROPERTY(EditAnywhere, Category = Tower)
    UStaticMeshComponent* Light;
  UPROPERTY(EditAnywhere, Category = Tower)
    FLinearColor ColorDisabled;
  UPROPERTY(EditAnywhere, Category = Tower)
    FLinearColor ColorEnabled;
  UPROPERTY(EditAnywhere, Category = Tower)
    bool NeedKey;
  UPROPERTY(EditAnywhere, Category = Tower)
    FLinearColor ColorKey;
  UPROPERTY(EditAnywhere, Category = Tower)
    float TimeRunes;
  UPROPERTY(EditAnywhere)
    UMaterialBillboardComponent *EffectsBB;
  UPROPERTY(EditAnywhere)
    UParticleSystemComponent *LightParticles;

  void RegisterDelegate();
  UFUNCTION()
    void OnBeginTriggerOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
  void EndPlay(const EEndPlayReason::Type EndPlayReason);
  void RestoreInitialState();
private:
  UMaterialInstanceDynamic *TowerLightMaterial;
  UMaterialInstanceDynamic *TowerEntranceMaterial;
  UMaterialInstanceDynamic *TowerRunesMaterial;
  UMaterialInstanceDynamic *MaterialBB;

  bool m_startVictory;
  float m_timeToFinish;
  float m_elapsedTime;
  float m_elapsedTimeRunes;
  float m_lightZAngle;
  FLinearColor m_origin;
  FLinearColor m_target;
  bool m_init;
};
