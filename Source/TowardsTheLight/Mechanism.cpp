// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "Mechanism.h"
#include "StaticPlatform.h"
#include "TimeManager.h"
#include "PlayerOvi.h"

AMechanism::AMechanism()
{
	PrimaryActorTick.bCanEverTick = true;
  //skeletal mesh
  SkeletalMesh = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalComponent"));
  if (SkeletalMesh) {
    SkeletalMesh->AlwaysLoadOnClient = true;
    SkeletalMesh->AlwaysLoadOnServer = true;
    SkeletalMesh->bOwnerNoSee = false;
    SkeletalMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
    SkeletalMesh->bCastDynamicShadow = true;
    SkeletalMesh->bAffectDynamicIndirectLighting = true;
    SkeletalMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
    SkeletalMesh->bChartDistanceFactor = true;
    SkeletalMesh->AttachParent = RootComponent;
    static FName CollisionProfileName(TEXT("OverlapAll"));
    SkeletalMesh->SetCollisionProfileName(CollisionProfileName);
    SkeletalMesh->bGenerateOverlapEvents = true;
    SkeletalMesh->bCanEverAffectNavigation = false;
    SkeletalMesh->SetRelativeLocation(FVector(0, 0, 0));
    SkeletalMesh->CastShadow = false;
  }
  //Public properties
  CanActivate = CanDisactivate = true;
  DisableAtEndAction = false;
  NumberOfActions = 1;
  TimeInIntermittence = 1.0f;
  TimeToStartIntermittence = 5.0f;
  ColorDisabled = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
  ColorEnabled = FLinearColor(0.0f, 0.9490f, 1.0f, 1.0f);
  intermitedOn = true;
  m_target = ColorEnabled;
  m_origin = ColorDisabled;
  //Private properties
  m_elapsedIntermitence = 0.0f;
  m_elapsedStartIntermitence = TimeToStartIntermittence;

  MechanismMaterial = ((UPrimitiveComponent*)GetRootComponent())->CreateAndSetMaterialInstanceDynamic(0);
  UMaterial* mat = nullptr;
  static ConstructorHelpers::FObjectFinder<UMaterial> MatFinder(TEXT("Material'/Game/Models/Button/Button.Button'"));
  if (MatFinder.Succeeded())
  {
    mat = MatFinder.Object;
    MechanismMaterial = UMaterialInstanceDynamic::Create(mat, GetWorld());
  }

  m_isPushed = false;
  m_justOff = false;
}

void AMechanism::BeginPlay()
{
	Super::BeginPlay();
  SkeletalMesh->SetMaterial(0, MechanismMaterial);
  int32 numTargets = Targets.Num();
  for (int32 i = 0; i < numTargets; i++)
    if (Targets[i] != nullptr)
      Targets[i]->InitByMechanism(DisableAtEndAction, NumberOfActions);

  m_target = (intermitedOn) ? ColorEnabled : ColorDisabled;
  m_origin = (!intermitedOn) ? ColorEnabled : ColorDisabled;
  m_isPushed = false;
  m_elapsedAnimation = 0.0f;
  m_justOff = false;
}

void AMechanism::Tick(float DeltaSeconds)
{
  DeltaSeconds = TimeManager::Instance()->GetDeltaTime(DeltaSeconds);
  Super::Tick(DeltaSeconds);
  m_elapsedStartIntermitence += DeltaSeconds;
  if (m_isPlayerOn) {
    m_justOff = false;
    if (m_elapsedStartIntermitence > TimeToStartIntermittence) {
      m_elapsedIntermitence += DeltaSeconds;
      float t = (m_elapsedIntermitence / TimeInIntermittence);
      t = (t > 1.0f) ? 1.0f : t;
      FLinearColor actual = FMath::Lerp(m_origin, m_target, t);
      MechanismMaterial->SetVectorParameterValue("Color", actual);
      if (m_elapsedIntermitence > TimeInIntermittence) {
        m_elapsedIntermitence = 0.0f;
        intermitedOn = !intermitedOn;
        m_target = (intermitedOn) ? ColorEnabled : ColorDisabled;
        m_origin = (!intermitedOn) ? ColorEnabled : ColorDisabled;
      }
    }
  }
  else {
    if (!m_justOff) {
      m_justOff = true;
      m_elapsedStartIntermitence = TimeToStartIntermittence;
      int32 numTargets = Targets.Num();
      bool some = false;
      for (int32 i = 0; i < numTargets; i++)
        if (Targets[i] != nullptr)
          if (Targets[i]->isEnabled()) {
            some = true;
            break;
          }
      FLinearColor actual = (some) ? ColorEnabled : ColorDisabled;
      m_target = (intermitedOn) ? ColorEnabled : ColorDisabled;
      m_origin = (!intermitedOn) ? ColorEnabled : ColorDisabled;
      MechanismMaterial->SetVectorParameterValue("Color", actual);
    }
  }
  
  //animation
  if (m_isPushed) {
    m_elapsedAnimation += DeltaSeconds;
    if (m_elapsedAnimation >= 1.0f) {
      m_isPushed = false;
      m_elapsedAnimation = 0.0f;
    }
  }
}

void AMechanism::Activate(bool enabled) {

}

void AMechanism::Execute() {
  if (!m_isPushed) {
    if (m_player) {
      m_player->EnabledPushButton();
      int32 numTargets = Targets.Num();

      bool canUse = true;
      bool prevEnabled = Targets[0]->isEnabled();
      for (int32 i = 0; i < numTargets; i++) {
        if (prevEnabled != Targets[i]->isEnabled()) {
          canUse = false;
          break;
        }
        prevEnabled = Targets[i]->isEnabled();
      }

      if (canUse) {
        for (int32 i = 0; i < numTargets; i++) {
          if (Targets[i]->isEnabled()) {
            if (CanDisactivate) {
              m_isPushed = true;
              m_elapsedStartIntermitence = 0.0f;
              MechanismMaterial->SetVectorParameterValue("Color", ColorDisabled);
              m_target = ColorEnabled;
              m_origin = ColorDisabled;
              Targets[i]->ChangeEnabled(false);
            }
          }
          else {
            if (CanActivate) {
              m_isPushed = true;
              m_elapsedStartIntermitence = 0.0f;
              MechanismMaterial->SetVectorParameterValue("Color", ColorEnabled);
              m_target = ColorDisabled;
              m_origin = ColorEnabled;
              Targets[i]->ChangeEnabled(true);
            }
          }
        }
      }
    }
  }

}

bool AMechanism::isPushingButton() {
  return m_isPushed;
}

void AMechanism::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  SkeletalMesh->SetMaterial(0, nullptr);
  MechanismMaterial = nullptr;
  Targets.Empty();
  SkeletalMesh = nullptr;
}
