// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "MobilePlatform.h"
#include "PlayerOvi.h"
#include "TimeManager.h"


AMobilePlatform::AMobilePlatform() {
  PrimaryActorTick.bCanEverTick = true;
  OurVisibleComponent->bGenerateOverlapEvents = true;
  this->SetActorEnableCollision(true);

  RootComponent->SetMobility(EComponentMobility::Movable);
  OurVisibleComponent->SetMobility(EComponentMobility::Movable);
  OurVisibleComponent->CastShadow = false;

  //setting
  RightDistance = 100.f;
  LeftDistance = 100.f;
  RightDelay = 1.f;
  LeftDelay = 1.f;
  Speed = 100.f;
  InitialDelay = 1.f;
  Enabled = true;
  ColorDisabled = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
  ColorEnabled = FLinearColor(0.0f, 0.9490f, 1.0f, 1.0f);
  TimeInIntermittence = 1.0f;

  //private variables
  m_timer = 0;
  m_state = INITIAL_DELAY;
  m_currentDistance = 0;
  m_maxActions = 1;
  m_actions = 0;
  m_disableAtEndAction = m_isPlayerOn = false;
  intermitedOn = true;
  m_target = ColorEnabled;
  m_origin = ColorDisabled;
  m_controlOff = false;

  MobilePlatformMaterial = ((UPrimitiveComponent*)GetRootComponent())->CreateAndSetMaterialInstanceDynamic(0);
  UMaterial* mat = nullptr;
  static ConstructorHelpers::FObjectFinder<UMaterial> MatFinder(TEXT("Material'/Game/Models/Plataforma_MOVIL/Plataforma_movil.Plataforma_movil'"));
  if (MatFinder.Succeeded())
  {
    mat = MatFinder.Object;
    MobilePlatformMaterial = UMaterialInstanceDynamic::Create(mat, GetWorld());
  }
}

void AMobilePlatform::BeginPlay() {
  Super::BeginPlay();
  this->Tags.Add("MobilePlatform");
  m_totalDistance = RightDistance;
  OurVisibleComponent->SetMaterial(0, MobilePlatformMaterial);
  m_target = (intermitedOn) ? ColorEnabled : ColorDisabled;
  m_origin = (!intermitedOn) ? ColorEnabled : ColorDisabled;
  FLinearColor color = (Enabled) ? ColorEnabled : ColorDisabled;
  MobilePlatformMaterial->SetVectorParameterValue("Color", color);

  //variables restore
  m_initialPosition = GetActorLocation();
  m_initialColor = color;
  m_enabledInitial = Enabled;
  m_controlOff = false;
}

void AMobilePlatform::Tick(float DeltaSeconds) {
  DeltaSeconds = TimeManager::Instance()->GetDeltaTime(DeltaSeconds);
  Super::Tick(DeltaSeconds);
  if (Enabled) {
    if (!m_player)
      for (TActorIterator< APawn > ActorItr(GetWorld()); ActorItr; ++ActorItr)
        if (ActorItr->ActorHasTag("Player")){
          m_player = (APlayerOvi*)*ActorItr;
          //break;
        }
    m_controlOff = false;
    doMovement(DeltaSeconds);
    if (m_player && m_isPlayerOn)
      m_player->OnMobilePlatform(this, m_movement);
  } else {
    if (!m_controlOff) {
      m_controlOff = true;
      MobilePlatformMaterial->SetVectorParameterValue("Color", ColorDisabled); // I think this functions is worst than an 'if'
    }
  }
}

void AMobilePlatform::doMovement(float DeltaSeconds){
  m_movement = FVector(0);
  switch (m_state){
  case INITIAL_DELAY:{
    m_elapsedIntermitence += DeltaSeconds;
    float t = (m_elapsedIntermitence / TimeInIntermittence);
    t = (t > 1.0f) ? 1.0f : t;
    FLinearColor actual = FMath::Lerp(m_origin, m_target, t);
    MobilePlatformMaterial->SetVectorParameterValue("Color", actual);
    if (m_elapsedIntermitence > TimeInIntermittence) {
      m_elapsedIntermitence = 0.0f;
      intermitedOn = !intermitedOn;
      m_target = (intermitedOn) ? ColorEnabled : ColorDisabled;
      m_origin = (!intermitedOn) ? ColorEnabled : ColorDisabled;
    }

    if (m_timer < InitialDelay)
      m_timer += DeltaSeconds;
    else{
      m_timer = 0;
      m_state = TO_RIGHT;
      m_rightVector = GetActorRightVector();
    }
  }
    break;
  case TO_RIGHT:{
    MobilePlatformMaterial->SetVectorParameterValue("Color", ColorEnabled);
    float dist = Speed * DeltaSeconds;
    if (m_totalDistance - m_currentDistance < dist)
      dist = m_totalDistance - m_currentDistance;

    if (m_currentDistance < m_totalDistance){
      m_movement = dist * m_rightVector;

      m_currentDistance += dist;
      FVector location = GetActorLocation();
      location += m_movement;
      SetActorLocation(location);
    }
    else{
      m_actions++;
      if (m_disableAtEndAction && m_actions >= m_maxActions) {
        Enabled = false;
        m_actions = 0;
      }
      m_state = RIGHT_DELAY;
      m_currentDistance = 0;
      m_elapsedIntermitence = 0.0f;
      m_target = ColorDisabled;
      m_origin = ColorEnabled;
      if (m_totalDistance == RightDistance)
        m_totalDistance += LeftDistance;
    }
  }
    break;
  case TO_LEFT:{
    MobilePlatformMaterial->SetVectorParameterValue("Color", ColorEnabled);
    float dist = Speed * DeltaSeconds;
    if (m_totalDistance - m_currentDistance < dist)
      dist = m_totalDistance - m_currentDistance;

    if (m_currentDistance < m_totalDistance){
      m_movement = dist * -m_rightVector;

      m_currentDistance += dist;
      FVector location = GetActorLocation();
      location += m_movement;
      SetActorLocation(location);
    }
    else{
       m_actions++;
       if (m_disableAtEndAction && m_actions >= m_maxActions) {
         Enabled = false;
         m_actions = 0;
       }

      m_state = LEFT_DELAY;
      m_currentDistance = 0;
      m_target = ColorDisabled;
      m_origin = ColorEnabled;
      m_elapsedIntermitence = 0.0f;
    }
  }
    break;
  case RIGHT_DELAY:{
    m_elapsedIntermitence += DeltaSeconds;
    float t = (m_elapsedIntermitence / TimeInIntermittence);
    t = (t > 1.0f) ? 1.0f : t;
    FLinearColor actual = FMath::Lerp(m_origin, m_target, t);
    MobilePlatformMaterial->SetVectorParameterValue("Color", actual);
    if (m_elapsedIntermitence > TimeInIntermittence) {
      m_elapsedIntermitence = 0.0f;
      intermitedOn = !intermitedOn;
      m_target = (intermitedOn) ? ColorEnabled : ColorDisabled;
      m_origin = (!intermitedOn) ? ColorEnabled : ColorDisabled;
    }
    if (m_timer < RightDelay)
      m_timer += DeltaSeconds;
    else{
      m_timer = 0;
      m_state = TO_LEFT;
    }
  }
    break;
  case LEFT_DELAY:{
    m_elapsedIntermitence += DeltaSeconds;
    float t = (m_elapsedIntermitence / TimeInIntermittence);
    t = (t > 1.0f) ? 1.0f : t;
    FLinearColor actual = FMath::Lerp(m_origin, m_target, t);
    MobilePlatformMaterial->SetVectorParameterValue("Color", actual);
    if (m_elapsedIntermitence > TimeInIntermittence) {
      m_elapsedIntermitence = 0.0f;
      intermitedOn = !intermitedOn;
      m_target = (intermitedOn) ? ColorEnabled : ColorDisabled;
      m_origin = (!intermitedOn) ? ColorEnabled : ColorDisabled;
    }
    if (m_timer < LeftDelay)
      m_timer += DeltaSeconds;
    else{
      m_timer = 0;
      m_state = TO_RIGHT;
    }
  }
    break;
  }
}

void AMobilePlatform::SetPlayerOn(bool on) {
  m_isPlayerOn = on;
}

void AMobilePlatform::ChangeEnabled(bool enabled) {
  Enabled = enabled;
}

bool AMobilePlatform::isEnabled() {
  return Enabled;
}

void AMobilePlatform::InitByMechanism(bool disableAtEnd, int32 numActions) {
  m_disableAtEndAction = disableAtEnd;
  m_maxActions = numActions;
}

void AMobilePlatform::RestoreInitialState() {
  SetActorLocation(m_initialPosition);
  MobilePlatformMaterial->SetVectorParameterValue("Color", m_initialColor);
  Enabled = m_enabledInitial;
  m_state = INITIAL_DELAY;
  m_totalDistance = RightDistance;
  m_actions = 0;
  m_isPlayerOn = false;
  intermitedOn = true;
  m_elapsedIntermitence = 0.0f;
  m_movement = FVector(0);
  m_currentDistance = 0;
}

void AMobilePlatform::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  OurVisibleComponent->SetMaterial(0, nullptr);
  MobilePlatformMaterial = nullptr;
  m_player = nullptr;
}