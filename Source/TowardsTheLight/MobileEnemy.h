// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StaticEnemy.h"
#include "MobileEnemy.generated.h"

#define COLLISION_ENEMY    ECC_GameTraceChannel2 
class APlayerOvi;

UCLASS()
class TOWARDSTHELIGHT_API AMobileEnemy : public AStaticEnemy
{
	GENERATED_BODY()
private:

  enum state{
    INITIAL_DELAY,
    TO_RIGHT,
    TO_LEFT,
    RIGHT_DELAY,
    LEFT_DELAY,
  }m_state;

  //animation
  bool m_isMoving;

  float m_timer;
  float m_totalDistance;
  float m_currentDistance;
  FVector m_rightVector;
  FVector m_rightPosition;
  FVector m_leftPosition;
  bool m_initMovement;

  float m_jumpSpeed;
  float m_accelerationJump;
  float m_actualJumpSpeed;
  float m_capsuleHeight;
  float m_capsuleRadious;
  float m_capsuleHeightPadding;
  float m_capsuleRadiousPadding;
  bool m_enableGravity;
  APlayerOvi *m_player;
  FVector m_lastPosition;
  FVector m_nextPosition;

  //kill variables
  int m_tickCounter;
  //restore data variables
  FTransform m_initialStatus;
  bool m_initialMovement;
  void doMovement(float DeltaSeconds);
  void CalculateGravity(float DeltaSeconds);
  void CheckCollision();
  void ResponseCollision();
  FVector AbsVector(const FVector& vector);
  FVector RecalculateLocation(FVector Direction, FVector Location, FVector HitLocation, float size);

public:
  UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  class USkeletalMeshComponent* EnemyAnimationMesh;
  UPROPERTY()
    UBoxComponent *Trigger;
  UPROPERTY(EditAnywhere, Category = MobileEnemy)
    bool Fly;
  UPROPERTY(EditAnywhere, Category = MobileEnemy)
    bool HasRotation;
  UPROPERTY(EditAnywhere, Category = MobileEnemy)
    float RightDistance;
  UPROPERTY(EditAnywhere, Category = MobileEnemy)
    float LeftDistance;
  UPROPERTY(EditAnywhere, Category = MobileEnemy)
    float RightDelay;
  UPROPERTY(EditAnywhere, Category = MobileEnemy)
    float LeftDelay;
  UPROPERTY(EditAnywhere, Category = MobileEnemy)
    float Speed;
  UPROPERTY(EditAnywhere, Category = MobileEnemy)
    float InitialDelay;
  UPROPERTY(EditAnywhere, Category = MobileEnemy)
    bool HasTrigger;
  UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
  class UCapsuleComponent* CapsuleComponent;

  AMobileEnemy();
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaSeconds) override;
  void RegisterDelegate();
  void EndPlay(const EEndPlayReason::Type EndPlayReason);
  virtual void RestoreInitialState();

  UFUNCTION()
    void OnBeginTriggerOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
  UFUNCTION(BlueprintCallable, Category = "EnemyLocomotion")
    bool EnemyisMoving();
};
