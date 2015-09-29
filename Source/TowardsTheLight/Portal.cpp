// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "Portal.h"
#include "PlayerOvi.h"
#include "TimeManager.h"

APortal::APortal(){
  PrimaryActorTick.bCanEverTick = true;
  m_activateTranslate = false;
  m_elapsedTranslate = 0.0f;
  m_isRunning = false;
  MeshBG = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshBG"));
  MeshBG->AttachTo(RootComponent);
  MeshBG->SetMobility(EComponentMobility::Static);
  MeshBG->CastShadow = false;

  MeshEffects = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshEffects"));
  MeshEffects->AttachTo(RootComponent);
  MeshEffects->SetMobility(EComponentMobility::Static);
  MeshEffects->CastShadow = false;

  ColorDisabled = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
  ColorEnabled = FLinearColor(0.0f, 0.9490f, 1.0f, 1.0f);
  PortalColor = FLinearColor(0.186f, 0.895f, 0.844f, 1.0f);

  PortalMaterial = ((UPrimitiveComponent*)GetRootComponent())->CreateAndSetMaterialInstanceDynamic(0);
  UMaterial* mat = nullptr;
  static ConstructorHelpers::FObjectFinder<UMaterial> MatFinder(TEXT("Material'/Game/Models/Portal/M_portalStructure.M_portalStructure'"));
  if (MatFinder.Succeeded()) {
    mat = MatFinder.Object;
    PortalMaterial = UMaterialInstanceDynamic::Create(mat, GetWorld());
  }

  PortalMaterialBG = ((UPrimitiveComponent*)GetRootComponent())->CreateAndSetMaterialInstanceDynamic(1);
  UMaterial* matBG = nullptr;
  static ConstructorHelpers::FObjectFinder<UMaterial> MatFinderBG(TEXT("Material'/Game/Models/Portal/M_portal_bg.M_portal_bg'"));
  if (MatFinderBG.Succeeded()) {
    matBG = MatFinderBG.Object;
    PortalMaterialBG = UMaterialInstanceDynamic::Create(matBG, GetWorld());
  }

  PortalMaterialEffects = ((UPrimitiveComponent*)GetRootComponent())->CreateAndSetMaterialInstanceDynamic(2);
  UMaterial* matEffects = nullptr;
  static ConstructorHelpers::FObjectFinder<UMaterial> MatFinderEffects(TEXT("Material'/Game/Models/Portal/M_portal_effect.M_portal_effect'"));
  if (MatFinderEffects.Succeeded()) {
    matEffects = MatFinderEffects.Object;
    PortalMaterialEffects = UMaterialInstanceDynamic::Create(matEffects, GetWorld());
  }

}

void APortal::BeginPlay() {
  Super::BeginPlay();
  m_elapsedTranslate = 0.0f;
  m_isRunning = false;
  MeshActivator->SetMaterial(0, PortalMaterial);
  MeshBG->SetMaterial(0, PortalMaterialBG);
  MeshEffects->SetMaterial(0, PortalMaterialEffects);
  EffectsBB->AddElement(MaterialBB, NULL, false, 100, 100, NULL);

  PortalMaterial->SetVectorParameterValue("Portal_structure_color", ColorDisabled);

  PortalMaterialBG->SetVectorParameterValue("Portal_BG_color", PortalColor);
  //if (NeedKey)
    PortalMaterialEffects->SetVectorParameterValue("Portal_effect_color", FLinearColor(FVector(0.f)));
  //else
    //PortalMaterialEffects->SetVectorParameterValue("Portal_effect_color", PortalColor);
}

void APortal::Tick(float DeltaSeconds) {
  DeltaSeconds = TimeManager::Instance()->GetDeltaTime(DeltaSeconds);
  Super::Tick(DeltaSeconds);
  if (m_activateTranslate && m_player) {
    m_elapsedTranslate += DeltaSeconds;
    if (m_elapsedTranslate > 0.7f) {
      m_activateTranslate = false;
      m_elapsedTranslate = 0.0f;
      FTransform newTransformPlayer = Partner->GetTransform();

      newTransformPlayer.SetScale3D(m_player->GetTransform().GetScale3D());

      float limitWorld = FVector::DotProduct(m_player->GetActorLocation(), m_player->GetActorRightVector());
      limitWorld = abs(limitWorld);
      FVector pForward = Partner->GetActorForwardVector();
      FVector pUp = Partner->GetActorUpVector();

      //position
      FVector location = newTransformPlayer.GetLocation();

      if (pForward.X > 0.05 || pForward.X < -0.05)
        location.X = (location.X > 0) ? limitWorld : -limitWorld;
      if (pForward.Y > 0.05 || pForward.Y < -0.05)
        location.Y = (location.Y > 0) ? limitWorld : -limitWorld;
      if (pForward.Z > 0.05 || pForward.Z < -0.05)
        location.Z = (location.Z > 0) ? limitWorld : -limitWorld;

      newTransformPlayer.SetLocation(location);

      //rotation
      FQuat q = newTransformPlayer.GetRotation() * FQuat::MakeFromEuler(FVector(0, 0, (m_player->PlayerisToRight()) ? 90 : -90));

      newTransformPlayer.SetRotation(q);

      m_player->SetActorTransform(newTransformPlayer);
      m_isRunning = false;
    }
  }
}

void APortal::Activate(bool enabled) {
  if (enabled){
    PortalMaterial->SetVectorParameterValue("Portal_structure_color", ColorEnabled);
    //if (NeedKey){
      PortalMaterialEffects->SetVectorParameterValue("Portal_effect_color", PortalColor);
      Partner->Turn(true);
    //}
  }
  else{
    PortalMaterial->SetVectorParameterValue("Portal_structure_color", ColorDisabled);
    //if (NeedKey){
      PortalMaterialEffects->SetVectorParameterValue("Portal_effect_color", FLinearColor(FVector(0.f)));
      Partner->Turn(false);
    //}
  }
}

void APortal::Execute() {
  FVector dif = m_player->GetActorUpVector() - MeshActivator->GetUpVector();
  dif.X = (dif.X < 0) ? -dif.X : dif.X;
  dif.Y = (dif.Y < 0) ? -dif.Y : dif.Y;
  dif.Z = (dif.Z < 0) ? -dif.Z : dif.Z;
  if (dif.X < 0.05 && dif.Y < 0.05 && dif.Z < 0.05 && !m_isRunning) {
    m_player->EnabledPickPortal();
    m_activateTranslate = true;
    m_elapsedTranslate = 0.0f;
    m_isRunning = true;
  }
}

void APortal::Turn(bool on){
  PortalMaterialEffects->SetVectorParameterValue("Portal_effect_color", on ? PortalColor : FLinearColor(FVector(0.f)));
}

void APortal::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  MeshActivator->SetMaterial(0, nullptr);
  MeshBG->SetMaterial(0, nullptr);
  MeshEffects->SetMaterial(0, nullptr);
  PortalMaterial = nullptr;
  PortalMaterialBG = nullptr;
  PortalMaterialEffects = nullptr;
  Partner = nullptr;
  MeshBG = nullptr;
  MeshEffects = nullptr;
}