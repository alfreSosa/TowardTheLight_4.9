// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PickableItem.generated.h"

UCLASS()
class TOWARDSTHELIGHT_API APickableItem : public AActor
{
	GENERATED_BODY()
	
public:	
	APickableItem();
  virtual void Tick(float DeltaSeconds) override;
  virtual void BeginPlay() override;
  virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
  void EndPlay(const EEndPlayReason::Type EndPlayReason);
  void RestoreInitialPosition();
  UPROPERTY(EditAnywhere)
    UStaticMeshComponent* OurVisibleComponent;
  UPROPERTY(EditAnywhere)
    UParticleSystemComponent *FeedBackParticles;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickableItem)
    float Points;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PickableItem)
    bool IsOrb;

  bool IsItemPicked();
private:
  const float DEFAULT_POINTS = 100.0f;
  bool m_collected;
  FVector m_initialPosition;
  int m_ciclesInRestore;
  float m_elapsedParticle;
};
