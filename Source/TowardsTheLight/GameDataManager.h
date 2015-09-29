// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

struct LevelData{
  FString name = "";
  float orbs = 0;
  float points = 0;
};

/**
 * https://answers.unrealengine.com/questions/205899/c-class-singleton-problem.html
 */
class TOWARDSTHELIGHT_API GameDataManager {
public:
  static GameDataManager* Instance();

  bool SavedGame();
  void DeleteGameData();

  LevelData ReadLevelData(FString levelName);
  void WriteLevelData(LevelData data);

  float GetOrbsCounts();
  float GetOrbsLevel(FString levelName);
  float GetPointsLevel(FString levelName);
  bool LevelExists(FString levelName);

  bool HasMusic();
  void SetMusic(bool enable);
  bool HasEffects();
  void SetEffects(bool enable);

  FString GetLanguage();
  void SetLanguage(FString language);

  FString GetPageName();
  void SetPageName(FString pageName);

protected:
  GameDataManager();
  ~GameDataManager();

private:
  static GameDataManager* m_instance;

  FString m_filePath = "StorageFiles/save/save.json";
  FString m_data;

  enum BinaryOption { NONE = 0, YES = 1, NO = -1 };
  BinaryOption m_music;
  BinaryOption m_effects;
  FString m_language;
  FString m_pageName;
};
