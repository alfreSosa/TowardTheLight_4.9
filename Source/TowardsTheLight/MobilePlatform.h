// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StaticPlatform.h"
#include "MobilePlatform.generated.h"

/**
 * 
 */
UCLASS()
class TOWARDSTHELIGHT_API AMobilePlatform : public AStaticPlatform
{
  GENERATED_BODY()
  
  class APlayerOvi *m_player;
  float m_timer;
  float m_totalDistance;
  float m_currentDistance;
  FVector m_rightVector;
  FVector m_movement;
  bool m_isPlayerOn;
  int32 m_actions;
  int32 m_maxActions;
  bool m_disableAtEndAction;
  UMaterialInstanceDynamic *MobilePlatformMaterial;
  float m_elapsedIntermitence;
  bool intermitedOn;
  FLinearColor m_target;
  FLinearColor m_origin;

  //variables for restore data
  FVector m_initialPosition;
  FLinearColor m_initialColor;
  bool m_enabledInitial;
  bool m_controlOff;

  enum state{
    INITIAL_DELAY,
    TO_RIGHT,
    TO_LEFT,
    RIGHT_DELAY,
    LEFT_DELAY,
  }m_state;

  void doMovement(float DeltaSeconds);
public:
  AMobilePlatform();
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaSeconds) override;
  void EndPlay(const EEndPlayReason::Type EndPlayReason);
  void SetPlayerOn(bool on);
  void ChangeEnabled(bool enabled);
  bool isEnabled();
  void InitByMechanism(bool disableAtEnd, int32 numActions);
  void RestoreInitialState();

  UPROPERTY(EditAnywhere, Category = MobilePlatform)
    float RightDistance;
  UPROPERTY(EditAnywhere, Category = MobilePlatform)
    float LeftDistance;
  UPROPERTY(EditAnywhere, Category = MobilePlatform)
    float RightDelay;
  UPROPERTY(EditAnywhere, Category = MobilePlatform)
    float LeftDelay;
  UPROPERTY(EditAnywhere, Category = MobilePlatform)
    float Speed;
  UPROPERTY(EditAnywhere, Category = MobilePlatform)
    float InitialDelay;
  UPROPERTY(EditAnywhere, Category = MechanismResponse)
    bool Enabled;
  UPROPERTY(EditAnywhere, Category = MechanismResponse)
    float TimeInIntermittence;
  UPROPERTY(EditAnywhere, Category = MechanismResponse)
    FLinearColor ColorDisabled;
  UPROPERTY(EditAnywhere, Category = MechanismResponse)
    FLinearColor ColorEnabled;
};
