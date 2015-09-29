// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "MyGameMode.generated.h"

/**
 * 
 */
class APickableItem;
class APlayerOvi;

UCLASS()
class TOWARDSTHELIGHT_API AMyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
  struct CheckPointData {
    bool IsPicked;
    FTransform PlayerStatus;
    bool PlayerHasKey;
    FLinearColor ColorKey;
    bool PlayerToRight;
    TArray<APickableItem *> ItemsPicked;
    float Points;
    float Orbs;
  };

  enum EndGameType { NONE = 0, VICTORY = 1, DEFEAT = -1 };

  AMyGameMode(const FObjectInitializer& ObjectInitializer);  // Our added constructor

  UFUNCTION()
    void AddPoints(float points);
  UFUNCTION()
    void OrbPicked();
  
  UFUNCTION(BlueprintCallable, Category = "GameState")
    float GetActualPoints();
  UFUNCTION(BlueprintCallable, Category = "GameState")
    float GetActualOrbs();

  void EndGame(EndGameType type);

  UFUNCTION(BlueprintCallable, Category = TTLFunctions)
    void SetPauseBP(bool enable);

  UFUNCTION(BlueprintCallable, Category = TTLFunctions)
    bool IsPausedBP();

  UFUNCTION(BlueprintCallable, Category = TTLFunctions)
    float EndGameBP();

  UFUNCTION(BlueprintCallable, Category = TTLFunctions)
    FString GetLevelNameBP();

  UFUNCTION(BlueprintCallable, Category = TTLFunctions)
    FString GetString(FString key);

  UFUNCTION(BlueprintCallable, Category = TTLFunctions)
    bool IsCheckPointPicked();

  UFUNCTION(BlueprintCallable, Category = TTLFunctions)
    void RestoreLevel(bool checkPoint);


  UFUNCTION(BlueprintImplementableEvent, Category = TTLEvents)
    void PointsSoundEvent();

  UFUNCTION(BlueprintImplementableEvent, Category = TTLEvents)
    void OrbsSoundEvent();

  //CheckPoint Functions
  void SetPlayerCheckPoint(APlayerOvi *player, FTransform playerStatus, bool right);
  void SetPlayerKey(bool hasKey, FLinearColor colorKey);
  void AddItemPicked(APickableItem *item);

private:
  CheckPointData m_actualCheckPoint;
  float m_actualPoints;
  float m_countOrbs;
  APlayerOvi *m_player;
  EndGameType state;
};

