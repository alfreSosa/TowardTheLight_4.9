// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "SoundManager.h"
#include "AudioDevice.h"

SoundManager* SoundManager::m_instance = nullptr;

SoundManager* SoundManager::Instance() {
  if (m_instance == nullptr)
    m_instance = new SoundManager();
  return m_instance;
}

SoundManager::SoundManager() {
}

SoundManager::~SoundManager() {
  delete m_instance;
  m_instance = nullptr;
}

void SoundManager::SetMusic(bool enable){
  FAudioDevice* Device = GEngine->GetMainAudioDevice();
  if (!Device)
    return;
  for (TMap<USoundClass*, FSoundClassProperties>::TIterator It(Device->SoundClasses); It; ++It){
    USoundClass* SoundClass = It.Key();
    if (SoundClass && SoundClass->GetFullName().Find("Music") != INDEX_NONE){
      SoundClass->Properties.Volume = enable? 1.f : 0.f;
    }
  }
}

void SoundManager::SetEffects(bool enable){
  FAudioDevice* Device = GEngine->GetMainAudioDevice();
  if (!Device)
    return;
  for (TMap<USoundClass*, FSoundClassProperties>::TIterator It(Device->SoundClasses); It; ++It){
    USoundClass* SoundClass = It.Key();
    if (SoundClass && SoundClass->GetFullName().Find("SFX") != INDEX_NONE){
      SoundClass->Properties.Volume = enable ? 1.f : 0.f;
    }
  }
}