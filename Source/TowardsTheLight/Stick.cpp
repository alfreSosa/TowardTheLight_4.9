// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "Stick.h"


AStick::AStick(){
  PrimaryActorTick.bCanEverTick = false;
  RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
  Stick = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
  Stick->SetMobility(EComponentMobility::Movable);
  Stick->CastShadow = false;
  Stick->AttachTo(RootComponent);
  UStaticMesh *mesh = nullptr;
  static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("StaticMesh'/Game/Models/Baculo/baculo_model.baculo_model'"));
  if (MeshFinder.Succeeded()){
    mesh = MeshFinder.Object;
    Stick->SetStaticMesh(mesh);
  }
  Stick->SetWorldScale3D(FVector(0.75, 0.75, 0.75));
  StickMaterial = ((UPrimitiveComponent*)GetRootComponent())->CreateAndSetMaterialInstanceDynamic(0);
  UMaterial* mat = nullptr;
  static ConstructorHelpers::FObjectFinder<UMaterial> MatFinder(TEXT("Material'/Game/Models/Baculo/baculo_diffuse.baculo_diffuse'"));
  if (MatFinder.Succeeded()){
    mat = MatFinder.Object;
    StickMaterial = UMaterialInstanceDynamic::Create(mat, GetWorld());
  }

  EffectsMaterial = ((UPrimitiveComponent*)GetRootComponent())->CreateAndSetMaterialInstanceDynamic(1);
  mat = nullptr;
  static ConstructorHelpers::FObjectFinder<UMaterial> MatFinderEffects(TEXT("Material'/Game/Models/Baculo/baculoBloom_material.baculoBloom_material'"));
  if (MatFinderEffects.Succeeded()){
    mat = MatFinderEffects.Object;
    EffectsMaterial = UMaterialInstanceDynamic::Create(mat, GetWorld());
  }

  BBMaterial = CreateDefaultSubobject<UMaterialBillboardComponent>(TEXT("BB"));
  BBMaterial->AttachTo(RootComponent);
  BBMaterial->SetMobility(EComponentMobility::Movable);
  BBMaterial->CastShadow = false;
  BBMaterial->SetRelativeLocation(FVector(0, 0, 70));
}

void AStick::BeginPlay(){
	Super::BeginPlay();
  this->Tags.Add("Stick");
  Stick->SetMaterial(0, StickMaterial);
  BBMaterial->AddElement(EffectsMaterial, NULL, false, 100, 100, NULL);
}

void AStick::SetColor(FLinearColor colorKey, float shinnes) {
  StickMaterial->SetScalarParameterValue("Brillo", shinnes);
  StickMaterial->SetVectorParameterValue("BaculoColor", colorKey);
  EffectsMaterial->SetVectorParameterValue("Bloom_Color", colorKey);
}

void AStick::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  Stick->SetMaterial(0, nullptr);
  StickMaterial = nullptr;
  EffectsMaterial = nullptr;
  Stick = nullptr;
}
