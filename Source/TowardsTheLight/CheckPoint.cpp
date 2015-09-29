#include "TowardsTheLight.h"
#include "CheckPoint.h"
#include "TowardsTheLightGameMode.h"
#include "PickableItem.h"
#include "PlayerOvi.h"

ACheckPoint::ACheckPoint()
{
  PrimaryActorTick.bCanEverTick = false;
  //initialize public properties
  RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
  CheckPointVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckPointVisibleComponent"));
  CheckPointVisibleComponent->AttachTo(RootComponent);
  CheckPointVisibleComponent->bGenerateOverlapEvents = true;
  CheckPointVisibleComponent->CastShadow = false;
  Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
  Trigger->SetCollisionProfileName(FName(TEXT("OverlapOnlyPawn")));
  Trigger->bHiddenInGame = true;
  Trigger->bGenerateOverlapEvents = true;
  Trigger->AttachTo(RootComponent);
  //initialize private properties
  m_loaded = false;
  m_enter = false;
  //particles
  FireParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PickedParticles"));
  FireParticles->bAutoActivate = false;
  static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/Models/CheckPoint/particulas/particulas_fuego_animadoazul2.particulas_fuego_animadoazul2"));
  if (ParticleAsset.Succeeded())
    FireParticles->SetTemplate(ParticleAsset.Object);
  FireParticles->AttachTo(CheckPointVisibleComponent);
}

void ACheckPoint::BeginPlay()
{
  Super::BeginPlay();
  RegisterDelegate();
  m_loaded = false;
  m_enter = false;
  m_gameMode = Cast<ATowardsTheLightGameMode>(UGameplayStatics::GetGameMode(this));
  FireParticles->SetActive(false);
}

void ACheckPoint::RegisterDelegate() 
{
  if (!Trigger->OnComponentBeginOverlap.IsAlreadyBound(this, &ACheckPoint::OnBeginTriggerOverlap))
    Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACheckPoint::OnBeginTriggerOverlap);
  
  if (!Trigger->OnComponentEndOverlap.IsAlreadyBound(this, &ACheckPoint::OnTriggerOverlapEnd))
    Trigger->OnComponentEndOverlap.AddDynamic(this, &ACheckPoint::OnTriggerOverlapEnd);
}

void ACheckPoint::OnBeginTriggerOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
  if (OtherActor->ActorHasTag("Player") && !m_loaded && !m_enter) {
    APlayerOvi *player = dynamic_cast<APlayerOvi *>(OtherActor);
    if (player) {
	  FVector dif = player->GetActorUpVector() - CheckPointVisibleComponent->GetUpVector();
	  dif.X = (dif.X < 0) ? -dif.X : dif.X;
	  dif.Y = (dif.Y < 0) ? -dif.Y : dif.Y;
	  dif.Z = (dif.Z < 0) ? -dif.Z : dif.Z;
	  if (dif.X < 0.05 && dif.Y < 0.05 && dif.Z < 0.05) {
		m_enter = true;
		FireParticles->SetActive(true);
		//storege information in checkpoint
		m_gameMode->SetPlayerCheckPoint(player, player->GetTransform(), player->PlayerisToRight());
		m_gameMode->SetPlayerKey(player->HasKey(), player->GetColorKey());
		for (TActorIterator<APickableItem> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		  if (ActorItr->IsItemPicked())
		    m_gameMode->AddItemPicked(*ActorItr);
	  }
    }
  }
}

void ACheckPoint::OnTriggerOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
  if (OtherActor->ActorHasTag("Player") && !m_loaded) 
    m_loaded = true;
}

void ACheckPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  if (Trigger->OnComponentBeginOverlap.IsAlreadyBound(this, &ACheckPoint::OnBeginTriggerOverlap))
    Trigger->OnComponentBeginOverlap.RemoveDynamic(this, &ACheckPoint::OnBeginTriggerOverlap);
 
  if (Trigger->OnComponentEndOverlap.IsAlreadyBound(this, &ACheckPoint::OnTriggerOverlapEnd))
    Trigger->OnComponentEndOverlap.RemoveDynamic(this, &ACheckPoint::OnTriggerOverlapEnd);

  FireParticles->DestroyComponent(true);
  m_gameMode = nullptr;
  Trigger = nullptr;
  CheckPointVisibleComponent = nullptr;
  FireParticles = nullptr;
  Super::EndPlay(EndPlayReason);
}

void ACheckPoint::RestoreInitialState() 
{
  m_loaded = false;
  m_enter = false;
  FireParticles->SetActive(false);
}


