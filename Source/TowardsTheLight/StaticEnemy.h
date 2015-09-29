// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "StaticEnemy.generated.h"

UCLASS()
class TOWARDSTHELIGHT_API AStaticEnemy : public AActor
{
	GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
    UStaticMeshComponent* OurVisibleComponent;

	AStaticEnemy();
  virtual void BeginPlay() override;
  void RegisterDelegate();
  void EndPlay(const EEndPlayReason::Type EndPlayReason);

  UFUNCTION()
    void OnCollision(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
  TScriptDelegate<FWeakObjectPtr> m_delegate;
};
