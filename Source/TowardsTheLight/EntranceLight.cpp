// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "EntranceLight.h"

AEntranceLight::AEntranceLight()
{
  PrimaryActorTick.bCanEverTick = false;
  RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
  LightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LigtMesh"));
  RootComponent->SetMobility(EComponentMobility::Static);
  LightMesh->SetMobility(EComponentMobility::Static);
  LightMesh->CastShadow = false;
  LightMesh->AttachTo(RootComponent);
  ColorKey = FLinearColor(0.0f, 0.0f, 0.0f);

  TowerLightMaterial = ((UPrimitiveComponent*)GetRootComponent())->CreateAndSetMaterialInstanceDynamic(0);
  UMaterial* mat = nullptr;
  static ConstructorHelpers::FObjectFinder<UMaterial> MatFinder(TEXT("Material'/Game/Models/Entrance/Entrance_light_additive.Entrance_light_additive'"));
  if (MatFinder.Succeeded())
  {
    mat = MatFinder.Object;
    TowerLightMaterial = UMaterialInstanceDynamic::Create(mat, GetWorld());
  }
}

void AEntranceLight::BeginPlay()
{
	Super::BeginPlay();
  LightMesh->SetMaterial(0, TowerLightMaterial);
  TowerLightMaterial->SetVectorParameterValue("TowerDoor_color", ColorKey);
}

void AEntranceLight::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  LightMesh->SetMaterial(0, nullptr);
  TowerLightMaterial = nullptr;
  LightMesh = nullptr;
}
