// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "StaticPlatform.h"

AStaticPlatform::AStaticPlatform() {
  PrimaryActorTick.bCanEverTick = false;
  RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
  OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
  RootComponent->SetMobility(EComponentMobility::Static);
  OurVisibleComponent->SetMobility(EComponentMobility::Static);
  OurVisibleComponent->CastShadow = false;
  OurVisibleComponent->bGenerateOverlapEvents = false;
  OurVisibleComponent->AttachTo(RootComponent);
  this->Tags.Add("Platform");
}

void AStaticPlatform::ChangeEnabled(bool enabled) {
}

bool AStaticPlatform::isEnabled() {
  return false;
}

void AStaticPlatform::InitByMechanism(bool disableAtEnd, int32 numActions) {
}
