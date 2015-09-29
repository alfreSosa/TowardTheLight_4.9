#include "TowardsTheLight.h"
#include "KeysTappable.h"


AKeysTappable::AKeysTappable()
{
  PrimaryActorTick.bCanEverTick = false;
  RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
  KeyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LigtMesh"));
  RootComponent->SetMobility(EComponentMobility::Static);
  KeyMesh->SetMobility(EComponentMobility::Static);
  KeyMesh->CastShadow = false;
  KeyMesh->AttachTo(RootComponent);
  ColorKey = FLinearColor(0.0f, 0.0f, 0.0f);

  TowerLightMaterial = ((UPrimitiveComponent*)GetRootComponent())->CreateAndSetMaterialInstanceDynamic(0);
  UMaterial* mat = nullptr;
  static ConstructorHelpers::FObjectFinder<UMaterial> MatFinder(TEXT("Material'/Game/Models/Color_Indicator/color_indicator_Mat.color_indicator_Mat'"));
  if (MatFinder.Succeeded())
  {
    mat = MatFinder.Object;
    TowerLightMaterial = UMaterialInstanceDynamic::Create(mat, GetWorld());
  }

}

void AKeysTappable::BeginPlay()
{
  Super::BeginPlay();
  KeyMesh->SetMaterial(0, TowerLightMaterial);
  TowerLightMaterial->SetVectorParameterValue("Portal_structure_color", ColorKey);
	
}

void AKeysTappable::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  KeyMesh->SetMaterial(0, nullptr);
  TowerLightMaterial = nullptr;
  KeyMesh = nullptr;
}

