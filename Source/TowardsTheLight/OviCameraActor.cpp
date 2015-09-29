// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "OviCameraActor.h"
#include "PlayerOvi.h"
#include "OviPlayerController.h"
#include "TimeManager.h"

AOviCameraActor::AOviCameraActor(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  //setting
  Padding = 200;
  DistanceCameraToPlayer = 2000;
  GetCameraComponent()->FieldOfView = 45;
  VerticalOffset = 500;
}

void AOviCameraActor::BeginPlay(){
  Super::BeginPlay();

  m_player = NULL;
}

void AOviCameraActor::Tick(float DeltaSeconds){
  DeltaSeconds = TimeManager::Instance()->GetDeltaTime(DeltaSeconds);
  Super::Tick(DeltaSeconds);

  if (!m_player)
    for (TActorIterator< APawn > ActorItr(GetWorld()); ActorItr; ++ActorItr) {
      if (ActorItr->ActorHasTag("Player")){
        m_player = (APlayerOvi*)*ActorItr;
        m_limitWorld = FVector::DotProduct(m_player->GetActorLocation(), m_player->GetActorRightVector());
        m_limitWorld = abs(m_limitWorld);
        m_limitPadding = m_limitWorld - Padding;
        m_cameraDistance = m_limitWorld + DistanceCameraToPlayer;
        m_ruleOfThree = (m_cameraDistance - m_limitPadding) / (m_limitWorld - m_limitPadding);

        AOviPlayerController* oviPlayerController = (AOviPlayerController*)GetWorld()->GetFirstPlayerController();
        if (oviPlayerController)
          oviPlayerController->SetViewTarget(this);

        //break;
      }
    }

  if (m_player){
    SetPosition();
    SetOrientation();
  }
}

void AOviCameraActor::SetPosition(){
  FVector finalPos = m_player->GetActorLocation();
  FVector dist(0, 0, 0);
  if (finalPos.X > m_limitPadding)
    dist.X = finalPos.X - m_limitPadding;
  else if (finalPos.X < -m_limitPadding)
    dist.X = finalPos.X + m_limitPadding;

  if (finalPos.Y > m_limitPadding)
    dist.Y = finalPos.Y - m_limitPadding;
  else if (finalPos.Y < -m_limitPadding)
    dist.Y = finalPos.Y + m_limitPadding;

  if (finalPos.Z > m_limitPadding)
    dist.Z = finalPos.Z - m_limitPadding;
  else if (finalPos.Z < -m_limitPadding)
    dist.Z = finalPos.Z + m_limitPadding;

  dist *= m_ruleOfThree;
  dist.Normalize();
  finalPos += dist * DistanceCameraToPlayer;

  finalPos += GetActorUpVector() * VerticalOffset;
  SetActorLocation(finalPos);
}

void AOviCameraActor::SetOrientation(){
  FVector forward = m_player->GetActorLocation() - this->GetActorLocation();
  forward.Normalize();
  FVector side = FVector::CrossProduct(m_player->GetActorUpVector(), forward);
  side.Normalize();

  FRotator rot = FRotationMatrix::MakeFromXY(forward, side).Rotator();
  this->SetActorRotation(rot);
}