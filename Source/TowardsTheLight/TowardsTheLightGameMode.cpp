// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "TowardsTheLightGameMode.h"
#include "PlayerOvi.h"
#include "OviPlayerController.h"
#include "GameDataManager.h"
#include "LocalizationManager.h"
#include "TimeManager.h"
#include "PickableItem.h"
#include "CheckPoint.h"
#include "MobilePlatform.h"
#include "StaticEnemy.h"
#include "MobileEnemy.h"
#include "MobilePlatform.h"
#include "Tower.h"
#include "IntermittentManager.h"
#include "IntermittentPlatform.h"
#include "Tutorial.h"
#include "SoundManager.h"
#include "InfoGameInstance.h"

#include "NoPawn.h"


ATowardsTheLightGameMode::ATowardsTheLightGameMode(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
  PrimaryActorTick.bCanEverTick = false;

  DefaultPawnClass = ANoPawn::StaticClass();
  PlayerControllerClass = AOviPlayerController::StaticClass();

  static ConstructorHelpers::FObjectFinder<UBlueprint> SomeBlueprint(TEXT("Blueprint'/Game/UIElementsBP/LevelHUD.LevelHUD'"));
  if (SomeBlueprint.Object)
    HUDClass = (UClass*)SomeBlueprint.Object->GeneratedClass;

  m_countOrbs = m_actualPoints = 0;
  state = EndGameType::NONE;

  //initialize checkpoint
  m_actualCheckPoint.IsPicked = false;
  m_player = nullptr;
  m_actualCheckPoint.PlayerHasKey = false;
  m_actualCheckPoint.ColorKey = FLinearColor(1, 1, 1);
}

void ATowardsTheLightGameMode::EndGame(EndGameType type) {
  switch (type){
  case VICTORY:{
    if (state == EndGameType::NONE){
      LevelData data = GameDataManager::Instance()->ReadLevelData(Cast<UInfoGameInstance>(GetGameInstance())->GetCurrentLevel());
      bool write = false;
      if (m_countOrbs >= data.orbs){
        data.orbs = m_countOrbs;
        write = true;
      }
      if (m_actualPoints >= data.points){
        data.points = m_actualPoints;
        write = true;
      }
      if (write)
        GameDataManager::Instance()->WriteLevelData(data);
    }

    state = EndGameType::VICTORY;
  }
    break;
  case DEFEAT:
    state = EndGameType::DEFEAT;
    break;
  }
}

void ATowardsTheLightGameMode::AddPoints(float points) {
  m_actualPoints += points;
  PointsSoundEvent();
}

void ATowardsTheLightGameMode::OrbPicked() {
  m_countOrbs++;
  OrbsSoundEvent();
}

float ATowardsTheLightGameMode::GetActualPoints() {
  return m_actualPoints;
}

float ATowardsTheLightGameMode::GetActualOrbs() {
  return m_countOrbs;
}

void ATowardsTheLightGameMode::SetPauseBP(bool enable) {
  TimeManager::Instance()->SetPause(enable);
}

bool ATowardsTheLightGameMode::IsPausedBP() {
  return TimeManager::Instance()->IsPaused();
}

float ATowardsTheLightGameMode::EndGameBP() {
  return (float)state;
}

FString ATowardsTheLightGameMode::GetLevelNameBP(){
  return Cast<UInfoGameInstance>(GetGameInstance())->GetCurrentLevel();
}

void ATowardsTheLightGameMode::SetPlayerCheckPoint(APlayerOvi *player, FTransform playerStatus, bool right) {
  //storage here points and orbs to avoid other functions
  m_actualCheckPoint.Points = m_actualPoints;
  m_actualCheckPoint.Orbs = m_countOrbs;
  m_actualCheckPoint.IsPicked = true;
  m_actualCheckPoint.PlayerToRight = right;
  m_actualCheckPoint.PlayerStatus = playerStatus;
  m_player = player;
}

void ATowardsTheLightGameMode::SetPlayerKey(bool hasKey, FLinearColor colorKey) {
  m_actualCheckPoint.PlayerHasKey = hasKey;
  m_actualCheckPoint.ColorKey = colorKey;
}

void ATowardsTheLightGameMode::AddItemPicked(APickableItem *item) {
  if (m_actualCheckPoint.ItemsPicked.Find(item) == INDEX_NONE)
    m_actualCheckPoint.ItemsPicked.Add(item);
}

bool ATowardsTheLightGameMode::IsCheckPointPicked() {
  return m_actualCheckPoint.IsPicked;
}

void ATowardsTheLightGameMode::RestoreLevel(bool checkPoint) {
  if (!m_player){
    for (TActorIterator< APawn > ActorItr(GetWorld()); ActorItr; ++ActorItr)
      if (ActorItr->ActorHasTag("Player"))
        m_player = (APlayerOvi*)*ActorItr;
  }
  if (checkPoint) {
    /*Si los he cogido y no estan en el array almacenado los restauro*/
    for (TActorIterator<APickableItem> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
      if (ActorItr->IsItemPicked()) {
        if (m_actualCheckPoint.ItemsPicked.Find(*ActorItr) == INDEX_NONE) {
          (*ActorItr)->RestoreInitialPosition();
        }
      }
    }
    //restauro enemigos
    for (TActorIterator< AMobileEnemy > eneItr(GetWorld()); eneItr; ++eneItr)
      eneItr->RestoreInitialState();
    //restauro mobiles
    for (TActorIterator< AMobilePlatform > movItr(GetWorld()); movItr; ++movItr)
      movItr->RestoreInitialState();

    m_actualPoints = m_actualCheckPoint.Points;
    m_countOrbs = m_actualCheckPoint.Orbs;
    m_player->ResetToCheckPoint(m_actualCheckPoint.PlayerStatus, m_actualCheckPoint.PlayerToRight);
    m_player->SetKey(m_actualCheckPoint.PlayerHasKey, m_actualCheckPoint.ColorKey);
    state = EndGameType::NONE;
  }
  else {
    m_player->ResetToCheckPoint(m_actualCheckPoint.InitialPlayerStatus, m_actualCheckPoint.InitialPlayerToRight);
    m_player->SetKey(false, FLinearColor(1, 1, 1));
    //restauro pickables
    for (TActorIterator< APickableItem > pickItr(GetWorld()); pickItr; ++pickItr)
      pickItr->RestoreInitialPosition();
    //restauro checkpoint
    for (TActorIterator< ACheckPoint > checkItr(GetWorld()); checkItr; ++checkItr)
      checkItr->RestoreInitialState();
    //restauro tutoriales
    for (TActorIterator< ATutorial> tutItr(GetWorld()); tutItr; ++tutItr)
      tutItr->RestoreInitialState();
    //restauro móviles
    for (TActorIterator< AMobilePlatform > movItr(GetWorld()); movItr; ++movItr)
      movItr->RestoreInitialState();
    //restauro enemigos
    for (TActorIterator< AMobileEnemy > eneItr(GetWorld()); eneItr; ++eneItr)
      eneItr->RestoreInitialState();
    //restauro intermitentes y managers
    for (TActorIterator< AIntermittentManager > manaItr(GetWorld()); manaItr; ++manaItr)
      manaItr->RestoreInitialState();
    for (TActorIterator< AIntermittentPlatform > interItr(GetWorld()); interItr; ++interItr)
      interItr->RestoreInitialState();
    //restauro torre
    for (TActorIterator<ATower> towItr(GetWorld()); towItr; ++towItr)
      (*towItr)->RestoreInitialState();

    m_actualCheckPoint.IsPicked = false;
    m_actualCheckPoint.Orbs = m_actualCheckPoint.Points = 0;
    m_countOrbs = 0.0f;
    m_actualPoints = 0.0f;

    state = EndGameType::NONE;

    m_actualCheckPoint.ItemsPicked.Empty();
    m_actualCheckPoint.PlayerHasKey = false;
    m_actualCheckPoint.ColorKey = FLinearColor(1, 1, 1);
  }
}

void ATowardsTheLightGameMode::RestoreGameMode() {
  m_countOrbs = 0.0f;
  m_actualPoints = 0.0f;
  state = EndGameType::NONE;

  m_actualCheckPoint.IsPicked = false;
  m_actualCheckPoint.Orbs = m_actualCheckPoint.Points = 0;
  m_actualCheckPoint.ItemsPicked.Empty();
  m_actualCheckPoint.PlayerHasKey = false;
  m_actualCheckPoint.ColorKey = FLinearColor(1, 1, 1);
}

float ATowardsTheLightGameMode::GetLevelOrbs(FString levelName) {
  return GameDataManager::Instance()->GetOrbsLevel(levelName);
}

float ATowardsTheLightGameMode::GetLevelPoints(FString levelName) {
  return GameDataManager::Instance()->GetPointsLevel(levelName);
}

float ATowardsTheLightGameMode::GetTotalOrbs(){
  return GameDataManager::Instance()->GetOrbsCounts();
}

bool ATowardsTheLightGameMode::LevelExists(FString levelName){
  return GameDataManager::Instance()->LevelExists(levelName);
}

bool ATowardsTheLightGameMode::HasMusicBP(){
  bool value = GameDataManager::Instance()->HasMusic();
  SoundManager::Instance()->SetMusic(value);
  return value;
}
void ATowardsTheLightGameMode::SetMusicBP(bool enable){
  GameDataManager::Instance()->SetMusic(enable);
  SoundManager::Instance()->SetMusic(enable);
}

bool ATowardsTheLightGameMode::HasEffectsBP(){
  bool value = GameDataManager::Instance()->HasEffects();
  SoundManager::Instance()->SetEffects(value);
  return value;
}
void ATowardsTheLightGameMode::SetEffectsBP(bool enable){
  GameDataManager::Instance()->SetEffects(enable);
  SoundManager::Instance()->SetEffects(enable);
}

FString ATowardsTheLightGameMode::GetString(FString key){
  return LocalizationManager::Instance()->GetString(key);
}

void ATowardsTheLightGameMode::ChangeLanguage(FString language){
  LocalizationManager::Instance()->SetLanguage(language);
  GameDataManager::Instance()->SetLanguage(language);
}

FString ATowardsTheLightGameMode::GetPageName(){
  return GameDataManager::Instance()->GetPageName();
}

void ATowardsTheLightGameMode::SetPageName(FString pageName){
  GameDataManager::Instance()->SetPageName(pageName);
}

void ATowardsTheLightGameMode::FindActualPlayer() {
  for (TActorIterator< APawn > ActorItr(GetWorld()); ActorItr; ++ActorItr)
    if (ActorItr->ActorHasTag("Player")) {
      m_player = (APlayerOvi*)*ActorItr;
      m_actualCheckPoint.InitialPlayerStatus = m_player->GetTransform();
      m_actualCheckPoint.InitialPlayerToRight = m_player->PlayerisToRight();
    }
}

void ATowardsTheLightGameMode::PlayerInTutorial(bool value){
  if (!m_player){
    for (TActorIterator< APawn > ActorItr(GetWorld()); ActorItr; ++ActorItr)
      if (ActorItr->ActorHasTag("Player"))
        m_player = (APlayerOvi*)*ActorItr;
  }

  m_player->inTutorial(value);
}

//
//void ATowardsTheLightGameMode::BeginPlay() {
//  for (TActorIterator< AActor > ActorItr(GetWorld()); ActorItr; ++ActorItr)
//    m_actores.Add(*ActorItr);
//}

void ATowardsTheLightGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  m_player = nullptr;
  GetWorld()->ForceGarbageCollection(true);
}