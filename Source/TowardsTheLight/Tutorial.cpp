#include "TowardsTheLight.h"
#include "Tutorial.h"
#include "TimeManager.h"
#include "TowardsTheLightGameMode.h"


ATutorial::ATutorial() {
  PrimaryActorTick.bCanEverTick = true;
  //initialize public properties
  AutoLoad = false;
  TimeAfterOut = 2.f;

  //trigger component
  TriggerIn = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerIn"));
  TriggerIn->SetCollisionProfileName(FName(TEXT("OverlapOnlyPawn")));
  TriggerIn->bHiddenInGame = true;
  TriggerIn->bGenerateOverlapEvents = true;
  TriggerIn->AttachTo(RootComponent);

  //initialize private properties
  //m_loaded = false;
  m_enter = false;
  m_gameMode = nullptr;
  //m_end = false;
  //m_currentTime = 0.f;
}

void ATutorial::BeginPlay() {
  Super::BeginPlay();
  RegisterDelegate();

  //m_loaded = false;
  m_enter = false;
}

void ATutorial::Tick(float DeltaSeconds) {
  //Get ATowardsTheLightGameMode
  if (!m_gameMode)
    m_gameMode = Cast<ATowardsTheLightGameMode>(UGameplayStatics::GetGameMode(this));

  DeltaSeconds = TimeManager::Instance()->GetDeltaTime(DeltaSeconds);
  Super::Tick(DeltaSeconds);

  if (AutoLoad && !m_enter) {
    m_enter = true;
    m_gameMode->EnterTutorialEvent(Key);
  }

  //if (m_loaded && !m_end){
  //  m_currentTime += DeltaSeconds;
  //  if (m_currentTime > TimeAfterOut){
  //    m_end = true;
  //    m_gameMode->ExitTutorialEvent(Key);
  //  }
  //}
}

void ATutorial::RegisterDelegate() {
  if (!TriggerIn->OnComponentBeginOverlap.IsAlreadyBound(this, &ATutorial::OnBeginTriggerOverlap)) {
    TriggerIn->OnComponentBeginOverlap.AddDynamic(this, &ATutorial::OnBeginTriggerOverlap);
  }

  //if (!TriggerIn->OnComponentEndOverlap.IsAlreadyBound(this, &ATutorial::OnTriggerOverlapEnd)) {
  //  TriggerIn->OnComponentEndOverlap.AddDynamic(this, &ATutorial::OnTriggerOverlapEnd);
  //}
}

void ATutorial::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  if (TriggerIn->OnComponentBeginOverlap.IsAlreadyBound(this, &ATutorial::OnBeginTriggerOverlap))  {
    TriggerIn->OnComponentBeginOverlap.RemoveDynamic(this, &ATutorial::OnBeginTriggerOverlap);
  }

  //if (TriggerIn->OnComponentEndOverlap.IsAlreadyBound(this, &ATutorial::OnTriggerOverlapEnd))  {
  //  TriggerIn->OnComponentEndOverlap.RemoveDynamic(this, &ATutorial::OnTriggerOverlapEnd);
  //}
  
  Super::EndPlay(EndPlayReason);
}

void ATutorial::RestoreInitialState(){
  //m_loaded = false;
  m_enter = false;
  //m_end = false;
  //m_currentTime = 0.f;
}

void ATutorial::OnBeginTriggerOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
  if (OtherActor->ActorHasTag("Player") && !m_enter){// && !m_loaded) {
    m_enter = true;

    m_gameMode->EnterTutorialEvent(Key);
  }
}

//void ATutorial::OnTriggerOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
//  if (OtherActor->ActorHasTag("Player") && !m_loaded && m_enter)
//    m_loaded = true;
//}