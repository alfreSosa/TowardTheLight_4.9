// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "MenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TOWARDSTHELIGHT_API AMenuGameMode : public AGameMode {
  GENERATED_BODY()

public:
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
};
