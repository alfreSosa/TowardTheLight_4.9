// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "StaticEnemy.h"
#include "TowardsTheLightGameMode.h"
#include "PlayerOvi.h"

// Sets default values
AStaticEnemy::AStaticEnemy() {
  PrimaryActorTick.bCanEverTick = false;
  //public properties
  RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
  OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
  OurVisibleComponent->SetMobility(EComponentMobility::Movable);
  OurVisibleComponent->AttachTo(RootComponent);
  OurVisibleComponent->bGenerateOverlapEvents = true;
  OurVisibleComponent->CastShadow = false;
  this->SetActorEnableCollision(true);

}

// Called when the game starts or when spawned
void AStaticEnemy::BeginPlay() {
	Super::BeginPlay();
  RegisterDelegate();
}

void AStaticEnemy::RegisterDelegate() {

  if (!OurVisibleComponent->OnComponentBeginOverlap.IsBound()) {
    m_delegate.BindUFunction(this, TEXT("OnCollision"));
    OurVisibleComponent->OnComponentBeginOverlap.Add(m_delegate);
  }
}

void AStaticEnemy::OnCollision(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
  if (OtherActor->ActorHasTag("Player")){
    ATowardsTheLightGameMode *gameMode = Cast<ATowardsTheLightGameMode>(UGameplayStatics::GetGameMode(this));
    if (gameMode)
      if (gameMode->EndGameBP() > -0.05)
        gameMode->EndGame(ATowardsTheLightGameMode::DEFEAT);
  }
}

void AStaticEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason){
  if (OurVisibleComponent->OnComponentBeginOverlap.IsBound())
    OurVisibleComponent->OnComponentBeginOverlap.Remove(m_delegate);

  OurVisibleComponent = nullptr;
  Super::EndPlay(EndPlayReason);
}