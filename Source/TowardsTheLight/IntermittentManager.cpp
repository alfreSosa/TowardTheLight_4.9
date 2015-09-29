// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "IntermittentManager.h"
#include "IntermittentPlatform.h"
#include "TimeManager.h"

AIntermittentManager::AIntermittentManager()
{
	PrimaryActorTick.bCanEverTick = true;
  Enabled = true;
  m_numPlatforms = 0;
  m_finishCounter = 0;
  m_someBlocked = false;
  m_restart = false;

}

void AIntermittentManager::BeginPlay()
{
	Super::BeginPlay();
  m_numPlatforms = Platforms.Num();
  m_finishCounter = m_numPlatforms;
  for(int32 i = 0; i < m_numPlatforms; i++) {
    if (Platforms[i] != nullptr) {
      Platforms[i]->InitOwner(this);
      //Platforms[i]->Init();
    }
  }
  m_someBlocked = false;
  m_restart = false;
  m_initialEnabled = Enabled;
}

void AIntermittentManager::Tick( float DeltaTime )
{
  DeltaTime = TimeManager::Instance()->GetDeltaTime(DeltaTime);
	Super::Tick( DeltaTime );
  
  if (m_finishCounter <= 0)
    Enabled = false;

  if (m_restart) {
    for (int32 i = 0; i < m_numPlatforms; i++)
      if (Platforms[i] != nullptr)
        Platforms[i]->Init();
    m_restart = false;
  }

  if (Enabled)
    if (!m_someBlocked)
      for (int32 i = 0; i < m_numPlatforms; i++)
        if (Platforms[i] != nullptr)
          Platforms[i]->TickManager(DeltaTime);

}

void AIntermittentManager::AlertBlocking(bool blocking) {
  m_someBlocked = blocking;
}

void AIntermittentManager::ChangeEnabled(bool enabled) {
  Enabled = enabled;
  if (Enabled) {
    m_restart = true;
    m_finishCounter = m_numPlatforms;
  }
}

bool AIntermittentManager::isEnabled() {
  return Enabled;
}

void AIntermittentManager::InitByMechanism(bool disableAtEnd, int32 numActions) {
  //Nothing
}

void AIntermittentManager::AlertFinish() {
  m_finishCounter--;
  m_finishCounter = (m_finishCounter <= 0) ? 0: m_finishCounter;
}

void AIntermittentManager::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  Platforms.Empty();
}
void AIntermittentManager::RestoreInitialState() {
  Enabled = m_initialEnabled;
  m_finishCounter = m_numPlatforms;
  m_someBlocked = false;
  m_restart = false;
}

