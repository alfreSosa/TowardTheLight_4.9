// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "PickableItem.h"
#include "TimeManager.h"
#include "TowardsTheLightGameMode.h"

// Sets default values
APickableItem::APickableItem() {
  PrimaryActorTick.bCanEverTick = true;
  RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
  OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
  OurVisibleComponent->CastShadow = false;
  OurVisibleComponent->AttachTo(RootComponent);

  //particulas
  //visible
  FeedBackParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PickedParticles"));
  FeedBackParticles->bAutoActivate = false;
  static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/Models/Coins/Pick_coin_part.Pick_coin_part"));
  if (ParticleAsset.Succeeded())
    FeedBackParticles->SetTemplate(ParticleAsset.Object);
  FeedBackParticles->AttachTo(RootComponent);

  this->SetActorEnableCollision(true);

  Points = DEFAULT_POINTS;
  m_collected = false;
  m_initialPosition = FVector::ZeroVector;
  m_ciclesInRestore = 5;
  m_elapsedParticle = 0.0f;

}

void APickableItem::BeginPlay() 
{
  m_initialPosition = GetActorLocation();
  m_ciclesInRestore = 5;
  FeedBackParticles->SetActive(false);
  m_elapsedParticle = 0.0f;
}


void APickableItem::Tick(float DeltaSeconds){
  DeltaSeconds = TimeManager::Instance()->GetDeltaTime(DeltaSeconds);
  Super::Tick(DeltaSeconds);
  if (m_ciclesInRestore >= 0)
    m_ciclesInRestore--;
  
  if (m_collected) {
    m_elapsedParticle += DeltaSeconds;
    FeedBackParticles->SetActive(true);
    OurVisibleComponent->SetVisibility(false);
    if (m_elapsedParticle >= 0.4f) {
      SetActorLocation(FVector(0, 0, 0));
      FeedBackParticles->SetActive(false);
    }
  }

  if (IsOrb){
    FRotator MyRot = GetActorRotation();
    MyRot.Yaw += 50 * DeltaSeconds;
    MyRot.Roll += 75 * DeltaSeconds;
    SetActorRotation(MyRot);
  }
}

void APickableItem::NotifyActorBeginOverlap(AActor* OtherActor) {
  if (OtherActor->ActorHasTag("Player") && !m_collected && m_ciclesInRestore < 0) {
    m_collected = true;
    this->SetActorEnableCollision(false);
    ATowardsTheLightGameMode *gameMode = Cast<ATowardsTheLightGameMode>(UGameplayStatics::GetGameMode(this));
    if (gameMode) {
      gameMode->AddPoints(this->Points);
      if (IsOrb)
        gameMode->OrbPicked();
    }
  }
}

bool APickableItem::IsItemPicked() {
  return m_collected;
}

void APickableItem::RestoreInitialPosition() {
  this->SetActorEnableCollision(true);
  PrimaryActorTick.bCanEverTick = true;
  m_collected = false;
  SetActorLocation(m_initialPosition);
  m_ciclesInRestore = 5;
  m_elapsedParticle = 0.0f;
  OurVisibleComponent->SetVisibility(true);
}

void APickableItem::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  FeedBackParticles->DestroyComponent(true);
  PrimaryActorTick.UnRegisterTickFunction();
}


