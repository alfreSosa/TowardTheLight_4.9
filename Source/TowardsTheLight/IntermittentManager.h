// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StaticPlatform.h"
#include "IntermittentManager.generated.h"

class AIntermittentPlatform;
UCLASS()
class TOWARDSTHELIGHT_API AIntermittentManager : public AStaticPlatform
{
	GENERATED_BODY()
	
public:	
  //properties
  UPROPERTY(EditAnywhere, Category = PlatformManaged)
    TArray<AIntermittentPlatform *> Platforms;
  UPROPERTY(EditAnywhere, Category = IntermittentPlatformState)
    bool Enabled;
  //functions
	AIntermittentManager();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
  void EndPlay(const EEndPlayReason::Type EndPlayReason);
  void ChangeEnabled(bool enabled);
  bool isEnabled();
  void InitByMechanism(bool disableAtEnd, int32 numActions);
  void AlertBlocking(bool blocking);
  void AlertFinish();
  void RestoreInitialState();

private:
  //properties
  int32 m_numPlatforms;
  int32 m_finishCounter;
  bool m_someBlocked;
  bool m_restart;
  bool m_initialEnabled;
};
