// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tappable.h"
#include "Mechanism.generated.h"

class AStaticPlatform;

UCLASS()
class TOWARDSTHELIGHT_API AMechanism : public ATappable
{
	GENERATED_BODY()
	
public:
  //Properties
  UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  class USkeletalMeshComponent* SkeletalMesh;
  UPROPERTY(EditAnywhere, Category = MechanismTarget)
    TArray<AStaticPlatform *> Targets;
  UPROPERTY(EditAnywhere, Category = MechanismTarget)
    bool CanActivate;
  UPROPERTY(EditAnywhere, Category = MechanismTarget)
    bool CanDisactivate;
  UPROPERTY(EditAnywhere, Category = MechanismResponse)
    bool DisableAtEndAction;
  UPROPERTY(EditAnywhere, Category = MechanismResponse)
    int32 NumberOfActions;
  UPROPERTY(EditAnywhere, Category = MechanismResponse)
    float TimeInIntermittence;
  UPROPERTY(EditAnywhere, Category = MechanismResponse)
    float TimeToStartIntermittence;
  UPROPERTY(EditAnywhere, Category = MechanismResponse)
    FLinearColor ColorDisabled;
  UPROPERTY(EditAnywhere, Category = MechanismResponse)
    FLinearColor ColorEnabled;

	//constructor
	AMechanism();

	//Class Functions
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
  void EndPlay(const EEndPlayReason::Type EndPlayReason);
  virtual void Activate(bool enabled);
  virtual void Execute();

  UFUNCTION(BlueprintCallable, Category = "ButtonLocomotion")
    bool isPushingButton();
private:
  //animation
  bool m_isPushed;
  float m_elapsedAnimation = 0.0f;
  
  //TargetVariables
  float m_elapsedIntermitence;
  float m_elapsedStartIntermitence;
  bool intermitedOn;
  FLinearColor m_target;
  FLinearColor m_origin;
  UMaterialInstanceDynamic *MechanismMaterial;
  bool m_justOff;
};
