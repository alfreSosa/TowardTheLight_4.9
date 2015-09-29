// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "InfoGameInstance.h"

UInfoGameInstance::UInfoGameInstance(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer){
  m_pageOpen = false;
  m_levelName = "";
  m_maxScore = 0.f;
  m_silverPercentage = 0.f;
  m_goldPercentage = 0.f;
  m_silverScore = 0.f;
  m_goldScore = 0.f;

  m_initialScreen = true;

  m_hasKey = false;
  m_colorKey = FLinearColor(0.0f, 0.0f, 0.0f);

  m_inMenu = true;
}

void UInfoGameInstance::SetPageOpen(){
  m_pageOpen = true;
}
bool UInfoGameInstance::IsPageOpen(){
  return m_pageOpen;
}

void UInfoGameInstance::SetCurrentLevelKeytitle(FString keyTitle){
	m_keyTitle = keyTitle;
}
FString UInfoGameInstance::GetCurrentLevelKeyTitle(){
	return m_keyTitle;
}

void UInfoGameInstance::SetCurrentLevel(FString levelName){
	m_levelName = levelName;
}
FString UInfoGameInstance::GetCurrentLevel(){
	return m_levelName;
}

void UInfoGameInstance::SetMaxScoreCurrentLevel(float maxScore){
  m_maxScore = maxScore;
  m_silverScore = maxScore * m_silverPercentage;
  m_goldScore = maxScore * m_goldPercentage;
}
float UInfoGameInstance::GetMaxScoreCurrentLevel(){
  return m_maxScore;
}

void UInfoGameInstance::SetSilverPercentage(float silverPercentage){
  m_silverPercentage = silverPercentage;
}
float UInfoGameInstance::GetSilverScore(){
  return m_silverScore;
}

void UInfoGameInstance::SetGoldPercentage(float goldPercentage){
  m_goldPercentage = goldPercentage;
}
float UInfoGameInstance::GetGoldScore(){
  return m_goldScore;
}

void UInfoGameInstance::SetInitialScreen(bool value){
  m_initialScreen = value;
}
bool UInfoGameInstance::GetInitialScreen(){
  return m_initialScreen;
}

void UInfoGameInstance::SetTowerNeedKey(bool hasKey){
  m_hasKey = hasKey;
}
bool UInfoGameInstance::GetTowerNeedKey(){
  return m_hasKey;
}
void UInfoGameInstance::SetTowerKeyColor(FLinearColor colorKey){
  m_colorKey = colorKey;
}
FLinearColor UInfoGameInstance::GetTowerKeyColor(){
  return m_colorKey;
}

void UInfoGameInstance::SetInMenu(bool inMenu){
  m_inMenu = inMenu;
}
bool UInfoGameInstance::GetInMenu(){
  return m_inMenu;
}