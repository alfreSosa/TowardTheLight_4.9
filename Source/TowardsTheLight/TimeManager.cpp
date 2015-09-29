// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "TimeManager.h"

TimeManager* TimeManager::m_instance = nullptr;

TimeManager* TimeManager::Instance() {
  if (m_instance == nullptr)
    m_instance = new TimeManager();
  return m_instance;
}

TimeManager::TimeManager() {
  m_paused = false;
}

TimeManager::~TimeManager() {
  delete m_instance;
  m_instance = nullptr;
}

void TimeManager::SetPause(bool paused){
  m_paused = paused;
}
bool TimeManager::IsPaused(){
  return m_paused;
}

float TimeManager::GetDeltaTime(float deltaTime){
  return m_paused ? 0 : deltaTime;
}