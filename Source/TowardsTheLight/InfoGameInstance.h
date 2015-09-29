// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "InfoGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TOWARDSTHELIGHT_API UInfoGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
  UInfoGameInstance(const FObjectInitializer& ObjectInitializer);

public:
  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    void SetPageOpen();
  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    bool IsPageOpen();

  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    void SetCurrentLevel(FString levelName);
  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    FString GetCurrentLevel();
	
  UFUNCTION(BlueprintCallable, Category = "GameInfo")
	  void SetCurrentLevelKeytitle(FString keyTitle);
  UFUNCTION(BlueprintCallable, Category = "GameInfo")
	  FString GetCurrentLevelKeyTitle();

  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    void SetMaxScoreCurrentLevel(float maxScore);
  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    float GetMaxScoreCurrentLevel();

  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    void SetSilverPercentage(float silverPercentage);
  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    float GetSilverScore();
  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    void SetGoldPercentage(float goldPercentage);
  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    float GetGoldScore();

  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    void SetInitialScreen(bool value);
  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    bool GetInitialScreen();

  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    void SetTowerNeedKey(bool hasKey);
  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    bool GetTowerNeedKey();
  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    void SetTowerKeyColor(FLinearColor colorKey);
  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    FLinearColor GetTowerKeyColor();

  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    void SetInMenu(bool inMenu);
  UFUNCTION(BlueprintCallable, Category = "GameInfo")
    bool GetInMenu();
private:
  bool m_pageOpen;
  FString m_pageName;
  FString m_levelName;
  FString m_keyTitle;
  float m_maxScore;
  float m_silverPercentage;
  float m_goldPercentage;
  float m_silverScore;
  float m_goldScore;

  bool m_initialScreen;

  bool m_hasKey;
  FLinearColor m_colorKey;

  bool m_inMenu;
};
