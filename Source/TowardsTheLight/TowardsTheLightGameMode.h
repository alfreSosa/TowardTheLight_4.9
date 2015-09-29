// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "TowardsTheLightGameMode.generated.h"

class APickableItem;
class ACheckPoint;
class AMobilePlatform;
class AMobileEnemy;
class APlayerOvi;
class UInfoGameInstance;
class AIntermittentManager;
class AIntermittentPlatform;
class ATutorial;

/**
 * 
 */
UCLASS()
class TOWARDSTHELIGHT_API ATowardsTheLightGameMode : public AGameMode
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

    //player restore
    FTransform InitialPlayerStatus;
    bool InitialPlayerToRight;
  };

  ATowardsTheLightGameMode(const FObjectInitializer& ObjectInitializer);
  //virtual void BeginPlay() override;
  void EndPlay(const EEndPlayReason::Type EndPlayReason);
  enum EndGameType { NONE = 0, VICTORY = 1, DEFEAT = -1 };
  void EndGame(EndGameType type);
  
  UFUNCTION()
    void AddPoints(float points);
  UFUNCTION()
    void OrbPicked();

  //function for initialize level
  UFUNCTION(BlueprintCallable, Category = TTLFunctions)
  void FindActualPlayer();

  UFUNCTION(BlueprintCallable, Category = TTLGameState)
    float GetActualPoints();
  UFUNCTION(BlueprintCallable, Category = TTLGameState)
    float GetActualOrbs();

  UFUNCTION(BlueprintImplementableEvent, Category = TTLEvents)
    void PointsSoundEvent();
  UFUNCTION(BlueprintImplementableEvent, Category = TTLEvents)
    void OrbsSoundEvent();

  UFUNCTION(BlueprintCallable, Category = TTLFunctions)
    void SetPauseBP(bool enable);
  UFUNCTION(BlueprintCallable, Category = TTLFunctions)
    bool IsPausedBP();
  UFUNCTION(BlueprintCallable, Category = TTLFunctions)
    float EndGameBP();

  UFUNCTION(BlueprintCallable, Category = TTLFunctions)
    FString GetLevelNameBP();

  //CheckPoint Functions
  UFUNCTION(BlueprintCallable, Category = TTLFunctions)
    bool IsCheckPointPicked();
  UFUNCTION(BlueprintCallable, Category = TTLFunctions)
    void RestoreLevel(bool checkPoint);
  UFUNCTION(BlueprintCallable, Category = TTLFunctions)
    void RestoreGameMode();
  void SetPlayerCheckPoint(APlayerOvi *player, FTransform playerStatus, bool right);
  void SetPlayerKey(bool hasKey, FLinearColor colorKey);
  void AddItemPicked(APickableItem *item);

  //menus
  UFUNCTION(BlueprintCallable, Category = TTLMenuFunctions)
    float GetLevelPoints(FString levelName);
  UFUNCTION(BlueprintCallable, Category = TTLMenuFunctions)
    float GetLevelOrbs(FString levelName);

  UFUNCTION(BlueprintCallable, Category = TTLMenuFunctions)
    float GetTotalOrbs();

  UFUNCTION(BlueprintCallable, Category = TTLMenuFunctions)
    bool LevelExists(FString levelName);

  UFUNCTION(BlueprintCallable, Category = TTLMenuFunctions)
    bool HasMusicBP();
  UFUNCTION(BlueprintCallable, Category = TTLMenuFunctions)
    void SetMusicBP(bool enable);
  UFUNCTION(BlueprintCallable, Category = TTLMenuFunctions)
    bool HasEffectsBP();
  UFUNCTION(BlueprintCallable, Category = TTLMenuFunctions)
    void SetEffectsBP(bool enable);

  UFUNCTION(BlueprintCallable, Category = TTLMenuFunctions)
    FString GetString(FString key);
  UFUNCTION(BlueprintCallable, Category = TTLMenuFunctions)
    void ChangeLanguage(FString language);

  UFUNCTION(BlueprintCallable, Category = TTLMenuFunctions)
    FString GetPageName();
  UFUNCTION(BlueprintCallable, Category = TTLMenuFunctions)
    void SetPageName(FString pageName);

  //Tutorial
  UFUNCTION(BlueprintImplementableEvent, Category = TTLEvents)
    void EnterTutorialEvent(const FString &Key);
  UFUNCTION(BlueprintImplementableEvent, Category = TTLEvents)
    void ExitTutorialEvent(const FString &Key);

  UFUNCTION(BlueprintCallable, Category = TTLEvents)
	void PlayerInTutorial(bool value);

private:
  CheckPointData m_actualCheckPoint;
  float m_actualPoints;
  float m_countOrbs;
  APlayerOvi *m_player;
  EndGameType state;
  //TArray<AActor *> m_actores;
};