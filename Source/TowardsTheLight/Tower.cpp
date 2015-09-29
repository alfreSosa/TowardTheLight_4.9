// Fill out your copyright notice in the Description page of Project Settings.

#include "TowardsTheLight.h"
#include "Tower.h"
#include "TowardsTheLightGameMode.h"
#include "PlayerOvi.h"
#include "TimeManager.h"
#include "InfoGameInstance.h"

// Sets default values
ATower::ATower() {
  PrimaryActorTick.bCanEverTick = true;
  RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
  Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));

  RootComponent->SetMobility(EComponentMobility::Static);
  Body->SetMobility(EComponentMobility::Static);
  Body->CastShadow = false;
  Body->AttachTo(RootComponent);
  this->Tags.Add("Platform");

  Light = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Light"));
  //Light->SetMobility(EComponentMobility::Static);
  Light->CastShadow = false;
  Light->AttachTo(Body);

  Entrance = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Entrance"));
  Entrance->SetMobility(EComponentMobility::Static);
  Entrance->CastShadow = false;
  Entrance->AttachTo(Body);

  Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
  Trigger->AttachTo(Entrance);
  Trigger->bHiddenInGame = true;
  Trigger->SetWorldScale3D(FVector(0.5, 0.5, 0.5));
  Trigger->bGenerateOverlapEvents = true;

  ColorDisabled = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
  ColorEnabled = FLinearColor(0.0f, 0.9490f, 1.0f, 1.0f);

  NeedKey = false;
  ColorKey = FLinearColor(0.0f, 0.0f, 0.0f);
  TimeRunes = 1.0f;

  TowerLightMaterial = ((UPrimitiveComponent*)GetRootComponent())->CreateAndSetMaterialInstanceDynamic(0);
  UMaterial* mat = nullptr;
  static ConstructorHelpers::FObjectFinder<UMaterial> MatFinder(TEXT("Material'/Game/Models/Tower/Particle_02/Crystal_Mat.Crystal_Mat'"));
  if (MatFinder.Succeeded())
  {
    mat = MatFinder.Object;
    TowerLightMaterial = UMaterialInstanceDynamic::Create(mat, GetWorld());
  }

  TowerRunesMaterial = ((UPrimitiveComponent*)GetRootComponent())->CreateAndSetMaterialInstanceDynamic(1);
  UMaterial* mat2 = nullptr;
  static ConstructorHelpers::FObjectFinder<UMaterial> MatRunesFinder(TEXT("Material'/Game/Models/Tower/torre.torre'"));
  if (MatRunesFinder.Succeeded())
  {
    mat2 = MatRunesFinder.Object;
    TowerRunesMaterial = UMaterialInstanceDynamic::Create(mat2, GetWorld());
  }

  MaterialBB = ((UPrimitiveComponent*)GetRootComponent())->CreateAndSetMaterialInstanceDynamic(2);
  mat = nullptr;
  static ConstructorHelpers::FObjectFinder<UMaterial> MatFinderEffectsBB(TEXT("Material'/Game/Models/Baculo/baculoBloom_material.baculoBloom_material'"));
  if (MatFinderEffectsBB.Succeeded()){
    mat = MatFinderEffectsBB.Object;
    MaterialBB = UMaterialInstanceDynamic::Create(mat, GetWorld());
  }

  TowerEntranceMaterial = ((UPrimitiveComponent*)GetRootComponent())->CreateAndSetMaterialInstanceDynamic(3);
  UMaterial* mat3 = nullptr;
  static ConstructorHelpers::FObjectFinder<UMaterial> MatEntranceFinder(TEXT("Material'/Game/Models/Entrance/Entrance_light_mat.Entrance_light_mat'"));
  if (MatEntranceFinder.Succeeded())
  {
    mat3 = MatEntranceFinder.Object;
    TowerEntranceMaterial = UMaterialInstanceDynamic::Create(mat3, GetWorld());
  }

  EffectsBB = CreateDefaultSubobject<UMaterialBillboardComponent>(TEXT("BB"));
  EffectsBB->AttachTo(RootComponent);
  EffectsBB->CastShadow = false;

  m_startVictory = false;
  m_timeToFinish = 2.0f;
  m_elapsedTime = 0.0f;
  m_elapsedTimeRunes = 0.0f;

  //particulas
  //visible
  LightParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PickedParticles"));
  LightParticles->bAutoActivate = false;
  static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/Models/Tower/Sphere_Light_Particle.Sphere_Light_Particle"));
  if (ParticleAsset.Succeeded())
    LightParticles->SetTemplate(ParticleAsset.Object);
  LightParticles->AttachTo(RootComponent);
  m_lightZAngle = 0.0f;
  m_init = false;
}

// Called when the game starts or when spawned
void ATower::BeginPlay() {
	Super::BeginPlay();

  LightParticles->SetActive(true);
  LightParticles->SetRelativeScale3D(FVector(0.2f));

  Light->SetMaterial(0, TowerLightMaterial);
  Body->SetMaterial(0, TowerRunesMaterial);
  Entrance->SetMaterial(0, TowerEntranceMaterial);

  TowerLightMaterial->SetVectorParameterValue("Color", ColorDisabled);
  TowerRunesMaterial->SetVectorParameterValue("ColorRunes", ColorKey);
  TowerEntranceMaterial->SetVectorParameterValue("TowerDoor_color", ColorKey);

  RegisterDelegate();
  m_startVictory = false;
  m_timeToFinish = 2.0f;
  m_elapsedTime = 0.0f;
  m_elapsedTimeRunes = 0.0f;
  EffectsBB->AddElement(MaterialBB, NULL, false, 100, 100, NULL);
  if (NeedKey)
    MaterialBB->SetVectorParameterValue("Bloom_Color", ColorKey);
  else
    MaterialBB->SetVectorParameterValue("Bloom_Color", FLinearColor(FVector(0.f)));

  m_origin = ColorKey;
  m_target = ColorDisabled;

  if (GetGameInstance()) {
    Cast<UInfoGameInstance>(GetGameInstance())->SetTowerNeedKey(NeedKey);
    Cast<UInfoGameInstance>(GetGameInstance())->SetTowerKeyColor(ColorKey);
    m_init = true;
  }

  Light->SetRelativeScale3D(FVector(0.5f));
}

void ATower::Tick(float DeltaSeconds) {
  if (!m_init) {
    Cast<UInfoGameInstance>(GetGameInstance())->SetTowerNeedKey(NeedKey);
    Cast<UInfoGameInstance>(GetGameInstance())->SetTowerKeyColor(ColorKey);
    m_init = true;
  }

  DeltaSeconds = TimeManager::Instance()->GetDeltaTime(DeltaSeconds);
  Super::Tick(DeltaSeconds);

  m_elapsedTimeRunes += DeltaSeconds;
  float x = m_elapsedTimeRunes / TimeRunes;
  x = (x > 1.0f) ? 1.0f : x;
  FLinearColor actual = FMath::Lerp(m_origin, m_target, x);
  TowerRunesMaterial->SetVectorParameterValue("ColorRunes", actual);
  if (m_elapsedTimeRunes >= TimeRunes) {
    m_elapsedTimeRunes = 0.0f;
    FLinearColor sav = m_origin;
    m_origin = m_target;
    m_target = sav;
  }

  if (m_startVictory) {
    float t = m_elapsedTime / m_timeToFinish;
    t = (t > 1.0f) ? 1.0f : t;
    LightParticles->SetRelativeScale3D(FVector(t));
    FLinearColor color = FMath::Lerp(ColorDisabled, ColorEnabled, t);
    TowerLightMaterial->SetVectorParameterValue("Color", color);

	Light->SetRelativeScale3D(FVector(t/2.f + 0.5f));

    m_elapsedTime += DeltaSeconds;
  }

  m_lightZAngle += 100 * DeltaSeconds;
  Light->SetRelativeRotation(FRotator::MakeFromEuler(FVector(0, 0, m_lightZAngle)));
}

void ATower::RegisterDelegate() {
  if (!Trigger->OnComponentBeginOverlap.IsAlreadyBound(this, &ATower::OnBeginTriggerOverlap)) {
    Trigger->OnComponentBeginOverlap.AddDynamic(this, &ATower::OnBeginTriggerOverlap);
  }
}

void ATower::OnBeginTriggerOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
  if (OtherActor->ActorHasTag("Player")){
    APlayerOvi *player = dynamic_cast<APlayerOvi *>(OtherActor);
    if (player){
      FVector dif = player->GetActorUpVector() - Entrance->GetUpVector();
      dif.X = (dif.X < 0) ? -dif.X : dif.X;
      dif.Y = (dif.Y < 0) ? -dif.Y : dif.Y;
      dif.Z = (dif.Z < 0) ? -dif.Z : dif.Z;
      if (dif.X < 0.05 && dif.Y < 0.05 && dif.Z < 0.05){
        if (!NeedKey || (NeedKey && player->HasKey() && ColorKey == player->GetColorKey())){
          ATowardsTheLightGameMode *gameMode = Cast<ATowardsTheLightGameMode>(UGameplayStatics::GetGameMode(this));
          if (gameMode) {
            m_startVictory = true;
            gameMode->EndGame(ATowardsTheLightGameMode::VICTORY);
          }
        }
      }
    }
  }
}

void ATower::EndPlay(const EEndPlayReason::Type EndPlayReason){
  if (Trigger->OnComponentBeginOverlap.IsAlreadyBound(this, &ATower::OnBeginTriggerOverlap))  {
    Trigger->OnComponentBeginOverlap.RemoveDynamic(this, &ATower::OnBeginTriggerOverlap);
  }

  Light->SetMaterial(0, nullptr);
  Body->SetMaterial(0, nullptr);
  Entrance->SetMaterial(0, nullptr);

  LightParticles->DestroyComponent(true);
  TowerLightMaterial = nullptr;
  TowerRunesMaterial = nullptr;
  TowerEntranceMaterial = nullptr;
  LightParticles = nullptr;
  Super::EndPlay(EndPlayReason);
}

void ATower::RestoreInitialState() {
  m_startVictory = false;
  TowerLightMaterial->SetVectorParameterValue("Color", ColorDisabled);
  m_timeToFinish = 2.0f;
  m_elapsedTime = 0.0f;
  LightParticles->SetRelativeScale3D(FVector(0.2f));
  Light->SetRelativeScale3D(FVector(0.5f));
}
