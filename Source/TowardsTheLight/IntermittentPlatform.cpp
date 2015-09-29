// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "IntermittentPlatform.h"
#include "IntermittentManager.h"
#include "TimeManager.h"

AIntermittentPlatform::AIntermittentPlatform() {
  this->PrimaryActorTick.bCanEverTick = true;
  OurVisibleComponent->bGenerateOverlapEvents = true;
  this->SetActorEnableCollision(true);

  RootComponent->SetMobility(EComponentMobility::Movable);
  OurVisibleComponent->SetMobility(EComponentMobility::Movable);
  OurVisibleComponent->CastShadow = false;

  //Init default properties
  //public
  IsLarge = false;
  NoUsesManager = false;
  NumberOfIntermitences = 0;
  InitialTimeDelay = TimeInStateNoVisible = TimeInStateVisible = 1.0f;
  TimeToStartFeedBack = 0.5f;
  EndTimeDelay = 0.0f;
  Loop = StartVisible = Enabled = true;
  //private
  m_countIntermittences = true;
  m_counterIntermittences = NumberOfIntermitences;
  m_elapsedTime = 0.0f;
  m_actualState = State::INITIALDELAY;
  m_playerIsTouching = false;
  m_owner = nullptr;

  //init default material
  IntermittentPlatformMaterial = ((UPrimitiveComponent*)GetRootComponent())->CreateAndSetMaterialInstanceDynamic(0);
  UMaterial* mat = nullptr;
  static ConstructorHelpers::FObjectFinder<UMaterial> MatFinder(TEXT("Material'/Game/Models/Plataforma_Intermit/Plat_intermit.Plat_intermit'"));
  if (MatFinder.Succeeded())
  {
    mat = MatFinder.Object;
    IntermittentPlatformMaterial = UMaterialInstanceDynamic::Create(mat, GetWorld());
  }

  IPMaterialLarge = ((UPrimitiveComponent*)GetRootComponent())->CreateAndSetMaterialInstanceDynamic(1);
  mat = nullptr;
  static ConstructorHelpers::FObjectFinder<UMaterial> MatFinder2(TEXT("Material'/Game/Models/Plataforma_Intermit/Plat_intermi_big.Plat_intermi_big'"));
  if (MatFinder2.Succeeded())
  {
    mat = MatFinder2.Object;
    IPMaterialLarge = UMaterialInstanceDynamic::Create(mat, GetWorld());
  }

}

void AIntermittentPlatform::BeginPlay()
{
  Super::BeginPlay();
  this->Tags.Add("IntermittentPlatform");
  if (IsLarge)
    OurVisibleComponent->SetMaterial(0, IPMaterialLarge);
  else
    OurVisibleComponent->SetMaterial(0, IntermittentPlatformMaterial);

  Init();
}


void AIntermittentPlatform::Init() 
{
  if (this->ActorHasTag("Platform"))
    this->Tags.Remove("Platform");
  m_actualState = State::INITIALDELAY;

  if (StartVisible) {
    if (IsLarge)
      IPMaterialLarge->SetScalarParameterValue("alpha_txt_inter", 0.0f);
    else
      IntermittentPlatformMaterial->SetScalarParameterValue("alpha_txt_inter", 0.0f);
    this->Tags.Add("Platform");
  }
  else {
    if (IsLarge)
      IPMaterialLarge->SetScalarParameterValue("alpha_txt_inter", 1.0f);
    else
      IntermittentPlatformMaterial->SetScalarParameterValue("alpha_txt_inter", 1.0f);
    this->Tags.Remove("Platform");
  }

  m_isVisible = StartVisible;
  m_playerIsTouching = false;
  m_counterIntermittences = NumberOfIntermitences;
  m_countIntermittences = (NumberOfIntermitences == 0) ? false : true;
  m_elapsedTime = 0.0f;
  m_finished = false;
}

void AIntermittentPlatform::Tick(float DeltaSeconds) {
  if (NoUsesManager) {
    DeltaSeconds = TimeManager::Instance()->GetDeltaTime(DeltaSeconds);
    Super::Tick(DeltaSeconds);

    if (m_countIntermittences && m_counterIntermittences <= 0)
      m_actualState = ENDDELAY;

    runStateMachine(DeltaSeconds);
  }
  else{
    this->PrimaryActorTick.UnRegisterTickFunction(); //this functions only works at tick function and it's the only way to remove the tick
    this->PrimaryActorTick.bCanEverTick = false;
    this->SetActorTickEnabled(false);
  }
}

void AIntermittentPlatform::TickManager(float DeltaSeconds) {
  if (m_countIntermittences && m_counterIntermittences <= 0)
    m_actualState = ENDDELAY;

  runStateMachine(DeltaSeconds);
}

void AIntermittentPlatform::runStateMachine(float DeltaSeconds) {
  if (Enabled) {
    m_elapsedTime += DeltaSeconds;
    switch (m_actualState)
    {
    case INITIALDELAY:
      if (m_elapsedTime > InitialTimeDelay) {
        m_actualState = (StartVisible) ? State::ON : State::OFF;
        m_elapsedTime = 0.0f;
      }
      break;
    case AIntermittentPlatform::ON:
      if (m_elapsedTime > TimeToStartFeedBack) {
        float t = m_elapsedTime / TimeInStateVisible;
        t = (t > 0.7f) ? 0.7f : t;
        if (IsLarge)
          IPMaterialLarge->SetScalarParameterValue("alpha_txt_inter", t);
        else
          IntermittentPlatformMaterial->SetScalarParameterValue("alpha_txt_inter", t);
      }

      if (m_elapsedTime > TimeInStateVisible) {
        if (m_countIntermittences)
          m_counterIntermittences--;

        m_isVisible = false;
        if (m_playerIsTouching)
          if (!NoUsesManager)
            if (m_owner != nullptr)
              m_owner->AlertBlocking(true);
       
        m_actualState = State::OFF;
        if (IsLarge)
          IPMaterialLarge->SetScalarParameterValue("alpha_txt_inter", 1.0f);
        else
          IntermittentPlatformMaterial->SetScalarParameterValue("alpha_txt_inter", 1.0f);
        this->Tags.Remove("Platform");
        m_elapsedTime = 0.0f;
      }
      break;
    case AIntermittentPlatform::OFF:
      if (m_elapsedTime > TimeInStateNoVisible) {
        if (m_countIntermittences)
          m_counterIntermittences--;

        m_isVisible = true;
        m_actualState = State::ON;
        if (IsLarge)
          IPMaterialLarge->SetScalarParameterValue("alpha_txt_inter", 0.0f);
        else
          IntermittentPlatformMaterial->SetScalarParameterValue("alpha_txt_inter", 0.0f);;
        this->Tags.Add("Platform");
        m_elapsedTime = 0.0f;
      }
      break;
    case AIntermittentPlatform::ENDDELAY:
      if (m_elapsedTime > EndTimeDelay) {
        m_elapsedTime = 0.0f;
        m_actualState = (Loop) ? INITIALDELAY : ENDDELAY;
        if (!Loop && !m_finished) {
          if (!NoUsesManager)
            if (m_owner != nullptr)
              m_owner->AlertFinish();
          m_finished = true;
        }
        m_isVisible = StartVisible;
        m_counterIntermittences = (Loop) ? NumberOfIntermitences : 0;        
      }
      break;
    default:
      break;
    }
  }
}

void AIntermittentPlatform::NotifyActorBeginOverlap(AActor* OtherActor) {
  if (OtherActor->ActorHasTag("Player")) {
    m_playerIsTouching = true;
    if (!m_isVisible) {
      if (!NoUsesManager)
        if (m_owner != nullptr)
          m_owner->AlertBlocking(true);
    }
  }
}

void AIntermittentPlatform::NotifyActorEndOverlap(AActor* OtherActor) {
  if (OtherActor->ActorHasTag("Player")) {
    m_playerIsTouching = false;
    if (!NoUsesManager)
      if (m_owner != nullptr)
        m_owner->AlertBlocking(false);
  }

}

void AIntermittentPlatform::InitOwner(AIntermittentManager *owner) {
  m_owner = owner;
}

void AIntermittentPlatform::RestoreInitialState() {
  Init();
}

void AIntermittentPlatform::ChangeEnabled(bool enabled) {
  Enabled = enabled;
  if (Enabled) {
    RestoreInitialState();
  }
}

bool AIntermittentPlatform::isEnabled() {
  return Enabled;
}

void AIntermittentPlatform::InitByMechanism(bool disableAtEnd, int32 numActions) {
  //Nothing
}

void AIntermittentPlatform::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  OurVisibleComponent->SetMaterial(0, nullptr);
  IntermittentPlatformMaterial = nullptr;
  IPMaterialLarge = nullptr;
  m_owner = nullptr;
}




