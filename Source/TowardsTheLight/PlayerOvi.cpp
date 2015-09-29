#include "TowardsTheLight.h"
#include "PlayerOvi.h"
#include "MobilePlatform.h"
#include "TimeManager.h"
#include "Stick.h"
#include "TowardsTheLightGameMode.h"
#include "Tappable.h"
#include "CheckPoint.h"
#include "Tutorial.h"

/************************************/
/*DEBUG ALTERNATIVO*/

/*if (GEngine)
{
GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Some variable values: x: %f, y: %f, z: %f "), forward.X, forward.Y, forward.Z));
}*/

/*************************************/
const float PADDING_COLLISION_PERCENT = 0.15f;
const float PADDING_COLLISION_PERCENT_FEET = 0.30f;
const float PADDING_COLLISION_PERCENT_RADIOUS = 0.55f;
const float DEFAULT_CAPSULE_RADIOUS = 30.0f;
const float CAPSULE_RADIOUS_PADDING = 5.0f;
const float DEFAULT_CAPSULE_HEIGHT = 95.0f;

const float DEFAULT_MOVEMENT_SPEED = 620.0f;
const float DEFAULT_JUMP_SPEED = 1550.0f;
const float DEFAULT_JUMP_ACC = 3300.0f;

APlayerOvi::APlayerOvi() {
  PrimaryActorTick.bCanEverTick = true;
  m_isValid = false;
  //Public properties
  FallSpeed = JumpSpeed = DEFAULT_JUMP_SPEED;
  MovementSpeed = DEFAULT_MOVEMENT_SPEED;
  AccelerationFall = AccelerationJump = DEFAULT_JUMP_ACC;
  MarginInput = 50.f;

  //Pawn properties
  AutoPossessPlayer = EAutoReceiveInput::Player0;

  bUseControllerRotationPitch = false;
  bUseControllerRotationRoll = false;
  bUseControllerRotationYaw = false;

  CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
  CapsuleComponent->InitCapsuleSize(DEFAULT_CAPSULE_RADIOUS, DEFAULT_CAPSULE_HEIGHT);

  static FName CollisionProfileName(TEXT("Pawn"));
  CapsuleComponent->SetCollisionProfileName(CollisionProfileName);

  CapsuleComponent->CanCharacterStepUpOn = ECB_No;
  CapsuleComponent->bShouldUpdatePhysicsVolume = true;
  CapsuleComponent->bCheckAsyncSceneOnMove = false;
  CapsuleComponent->bCanEverAffectNavigation = false;
  CapsuleComponent->bGenerateOverlapEvents = true;
  //Fijamos por defecto la rotacion de la capsula para que el forward este de cara a la camara
  CapsuleComponent->SetRelativeRotation(FRotator::MakeFromEuler(FVector(0, 0, 0)));
  CapsuleComponent->SetCapsuleHalfHeight(DEFAULT_CAPSULE_HEIGHT);
  CapsuleComponent->SetMaterial(0, nullptr); //WARNING DE MANOLO
  RootComponent = CapsuleComponent;

#if WITH_EDITORONLY_DATA
  ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
  if (ArrowComponent) {
    ArrowComponent->ArrowColor = FColor(150, 200, 255);
    ArrowComponent->bTreatAsASprite = true;
    ArrowComponent->AttachParent = CapsuleComponent;
    ArrowComponent->bIsScreenSizeScaled = true;
  }
#endif // WITH_EDITORONLY_DATA

  Mesh = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalComponent"));
  if (Mesh) {
    Mesh->AlwaysLoadOnClient = true;
    Mesh->AlwaysLoadOnServer = true;
    Mesh->bOwnerNoSee = false;
    Mesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
    Mesh->bCastDynamicShadow = true;
    Mesh->bAffectDynamicIndirectLighting = true;
    Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
    Mesh->bChartDistanceFactor = true;
    Mesh->AttachParent = CapsuleComponent;
    static FName CollisionProfileName(TEXT("OverlapAll"));
    Mesh->SetCollisionProfileName(CollisionProfileName);
    Mesh->bGenerateOverlapEvents = true;
    Mesh->bCanEverAffectNavigation = false;
    Mesh->SetRelativeLocation(FVector(0, 0, 0));
    Mesh->SetRelativeRotation(FRotator::MakeFromEuler(FVector(0, 0, 90)));
    Mesh->CastShadow = false;
  }

  //private properties
  //key propierties
  m_hasKey = false;
  m_colorKey = FLinearColor(0.0f, 0.0f, 0.0f);
  m_stick = nullptr;

  //player movement
  m_state = States::RIGHT;
  m_limit = 0;
  m_right = m_left = false;

  //player jump & gravity
  m_isJumping = m_doJump = m_hasLanded = m_headCollision = false;
  m_enabledGravity = true;
  m_actualJumpSpeed = JumpSpeed;

  //pawn capsule private propiertes
  m_capsuleHeight = DEFAULT_CAPSULE_HEIGHT;
  m_capsuleRadious = DEFAULT_CAPSULE_RADIOUS;
  m_capsuleHeightPadding = m_capsuleHeight * PADDING_COLLISION_PERCENT;
  m_capsuleRadiousPadding = m_capsuleRadious * PADDING_COLLISION_PERCENT_RADIOUS;

  //input properties
  m_limitViewPort0 = 0.f;
  m_limitViewPort1 = 0.f;
  m_centerTouchX = 0.f;
  m_fingerIndexMovement = ETouchIndex::Touch10;
  m_fingerIndexJump = ETouchIndex::Touch10;
  m_fingerIndexOther = ETouchIndex::Touch10;

  m_stateInput = States::STOP;

  //iteraction mobile platform propierties
  bPlayerRunning = false;
  m_currentMobile = nullptr;
  m_isOnMobilePlatform = false;
  //iteraction Tappables propierties
  m_isPickingAltar = m_isPickingPortal = m_isPushingButton = false;
  m_elapsedAltar = m_elapsedPortal = m_elapsedButton = 0.0f;
  m_inTutorial = false;
  m_gameMode = nullptr;
}

void APlayerOvi::BeginPlay() {
  Super::BeginPlay();
  //tag player
  this->Tags.Add("Player");

  // time for button animation
  m_elapsedAltar = m_elapsedPortal = m_elapsedButton = 0.0f;
  // material for capsule
  CapsuleComponent->SetMaterial(0, nullptr);
  // calculate limit world & initial orientation
  float dotForward = FVector::DotProduct(FVector(1, 1, 1), GetActorForwardVector());
  if (dotForward < 0)
    m_state = States::LEFT;
  else
    m_state = States::RIGHT;

  m_limit = FVector::DotProduct(GetActorLocation(), GetActorRightVector());
  m_limit = abs(m_limit);

  //GetCaspuleValues for simulate physics
  m_capsuleHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
  m_capsuleRadious = CapsuleComponent->GetScaledCapsuleRadius();
  m_capsuleHeightPadding = m_capsuleHeight * PADDING_COLLISION_PERCENT;
  m_capsuleRadiousPadding = m_capsuleRadious * PADDING_COLLISION_PERCENT_RADIOUS;
  m_capsuleHeightPaddingFeet = m_capsuleHeight * PADDING_COLLISION_PERCENT_FEET;

  //Set player Stick in the animation socket
  if (Mesh) {
    m_stick = GetWorld()->SpawnActor<AStick>(AStick::StaticClass());
    const USkeletalMeshSocket *socket = Mesh->GetSocketByName("Puntodeacople_Baston");
    if (socket)
      socket->AttachActor(m_stick, Mesh);
    m_colorKey = FLinearColor(FVector(0.5f));
    m_stick->SetColor(FLinearColor(FVector(0.5f)), 5.0f);
  }

  m_inTutorial = false;
  m_isValid = false;
}

void APlayerOvi::Tick(float DeltaSeconds){
  //get own deltaTime
  DeltaSeconds = TimeManager::Instance()->GetDeltaTime(DeltaSeconds);
  Super::Tick(DeltaSeconds);

  //initilize raycast query param
  //Initialize TraceParam
  if (!m_isValid) {
    //Get ATowardsTheLightGameMode
    if (!m_gameMode)
      m_gameMode = Cast<ATowardsTheLightGameMode>(UGameplayStatics::GetGameMode(this));

    m_gameMode->FindActualPlayer();

    static FName FireTraceIdent = FName(TEXT("Platform"));
    FCollisionQueryParams TraceParams(FireTraceIdent, true, this);
    TraceParams.bTraceAsyncScene = true;
    TraceParams.bFindInitialOverlaps = false;
    TraceParams.bTraceComplex = true;

    TArray<AActor *> ignorados;
    for (TActorIterator<ATappable > ActorItr(GetWorld()); ActorItr; ++ActorItr) {
      if (ActorItr->ActorHasTag("Tappable"))
        ignorados.Add(*ActorItr);
    }

    for (TActorIterator<ACheckPoint > checkItr(GetWorld()); checkItr; ++checkItr)
      ignorados.Add(*checkItr);

    for (TActorIterator<ATutorial > tutItr(GetWorld()); tutItr; ++tutItr)
      ignorados.Add(*tutItr);
    
    if (ignorados.Num() > 0)
      TraceParams.AddIgnoredActors(ignorados);

    m_TraceParams = TraceParams;
    m_isValid = true;
  }

  //pause animations if is game paused
  Mesh->bPauseAnims = isPlayerPaused();

  //first tick initialize viewport properties, dont work at begin play
  if (m_limitViewPort0 == 0 && m_limitViewPort1 == 0) {
    m_limitViewPort0 = GEngine->GameViewport->Viewport->GetSizeXY().X * 0.45;
    m_limitViewPort1 = GEngine->GameViewport->Viewport->GetSizeXY().X * 0.55;
  }

  //update elapsed time if push button animation is running
  if (m_isPushingButton) {
    m_elapsedButton += DeltaSeconds;
    if (m_elapsedButton >= 1.0f) {
      m_elapsedButton = 0.0f;
      m_isPushingButton = false;
    }
  }

  if (m_isPickingPortal) {
    m_elapsedPortal += DeltaSeconds;
    float t = 1.0f - (m_elapsedPortal / 1.2f);
    Mesh->SetRelativeScale3D(FVector(t, t, t));
    if (m_elapsedPortal >= 0.6f) {
      m_elapsedPortal = 0.0f;
      m_isPickingPortal = false;
      Mesh->SetRelativeScale3D(FVector(1, 1, 1));
    }
  }

  if (m_isPickingAltar) {
    m_elapsedAltar += DeltaSeconds;
    if (m_elapsedAltar >= 0.7f) {
      m_elapsedAltar = 0.0f;
      m_isPickingAltar = false;
    }
  }

  //get last position for this frame.
  m_lastPosition = GetActorLocation();
  float value = 0.0f;
  if (isInputEnabled())
    value = UpdateState();
  else{
    value = 0;
    m_doJump = false;
  }

  DoMovement(DeltaSeconds, value);
  if (!m_isPickingPortal) {
    DoJump(DeltaSeconds);
    CalculateGravity(DeltaSeconds);
  }
  CheckCollision();
  CalculateOrientation();
}

float APlayerOvi::UpdateState() {
  //update movement
  float value = 0;
  if (m_right){
    value = 1;
    if (m_state != States::RIGHT){
      Rotate(FVector(0, 0, -180));
      m_state = States::RIGHT;
    }

  }
  else if (m_left){
    value = 1;
    if (m_state != States::LEFT){
      Rotate(FVector(0, 0, 180));
      m_state = States::LEFT;
    }
  }
  else{
    value = 0;
  }

  //update animation variable
  bPlayerRunning = (value != 0) ? true : false;

  return value;
}

void APlayerOvi::SetupPlayerInputComponent(class UInputComponent* InputComponent) {
  Super::SetupPlayerInputComponent(InputComponent);
  InputComponent->BindAction("MoveRight", IE_Pressed, this, &APlayerOvi::OnStartRight);
  InputComponent->BindAction("MoveRight", IE_Released, this, &APlayerOvi::OnStopRight);
  InputComponent->BindAction("MoveLeft", IE_Pressed, this, &APlayerOvi::OnStartLeft);
  InputComponent->BindAction("MoveLeft", IE_Released, this, &APlayerOvi::OnStopLeft);
  InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerOvi::OnStartJump);
  InputComponent->BindAction("Jump", IE_Released, this, &APlayerOvi::OnStopJump);

  InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &APlayerOvi::TouchStarted);
  InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &APlayerOvi::TouchStarted);
  InputComponent->BindTouch(EInputEvent::IE_Released, this, &APlayerOvi::TouchEnd);
}

void APlayerOvi::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location) {
  if (Location.X > m_limitViewPort1 && m_fingerIndexMovement != FingerIndex && m_fingerIndexOther != FingerIndex) { //JUMP
    OnStartJump();
    m_fingerIndexJump = FingerIndex;
  }
  else if (Location.X < m_limitViewPort0 && m_fingerIndexJump != FingerIndex && m_fingerIndexOther != FingerIndex){ //MOVEMENT SWIPE
    if (m_centerTouchX == 0){ //initial touch
      m_centerTouchX = Location.X;
      m_fingerIndexMovement = FingerIndex;
    }
    if (m_fingerIndexMovement == FingerIndex){
      if (Location.X > m_centerTouchX + MarginInput){ //right swipe
        OnStartRight();
        OnStopLeft();
        m_centerTouchX = Location.X - MarginInput;
        m_stateInput = States::RIGHT;
      }
      else if (Location.X < m_centerTouchX - MarginInput){ //left swipe
        OnStartLeft();
        OnStopRight();
        m_centerTouchX = Location.X + MarginInput;
        m_stateInput = States::LEFT;
      }
      else if (Location.X > m_centerTouchX && m_stateInput == States::RIGHT){ //right swipe
        OnStartRight();
        OnStopLeft();
      }
      else if (Location.X < m_centerTouchX && m_stateInput == States::LEFT){ //left swipe
        OnStartLeft();
        OnStopRight();
      }
      else{ //no movement
        OnStopRight();
        OnStopLeft();
        m_stateInput = States::STOP;
      }
    }
  }
  else if (m_fingerIndexJump != FingerIndex && m_fingerIndexMovement != FingerIndex)
    m_fingerIndexOther = FingerIndex;

}

void APlayerOvi::TouchEnd(const ETouchIndex::Type FingerIndex, const FVector Location) {
  if (FingerIndex == m_fingerIndexJump){
    OnStopJump();
    m_fingerIndexJump = ETouchIndex::Touch10;
  }
  else if (FingerIndex == m_fingerIndexMovement){
    m_centerTouchX = 0.f;
    m_fingerIndexMovement = ETouchIndex::Touch10;
    OnStopRight();
    OnStopLeft();
  }
  else if (FingerIndex == m_fingerIndexOther)
    m_fingerIndexOther = ETouchIndex::Touch10;
}

void APlayerOvi::OnStartRight() {
  m_right = true;
}

void APlayerOvi::OnStopRight() {
  m_right = false;
}

void APlayerOvi::OnStartLeft() {
  m_left = true;
}

void APlayerOvi::OnStopLeft() {
  m_left = false;
}

void APlayerOvi::OnStartJump() {
  m_doJump = true;
}

void APlayerOvi::OnStopJump() {
  m_doJump = false;
}

void  APlayerOvi::DoMovement(float DeltaSeconds, float value){
  FVector forward = GetActorForwardVector();
  FVector location = GetActorLocation();
  location += MovementSpeed * DeltaSeconds * value * forward;
  SetActorLocation(location);
}

void APlayerOvi::DoJump(float DeltaSeconds){
  if (m_hasLanded && !m_doJump) {
    m_isJumping = false;
    m_headCollision = false;
  }

  if (m_doJump && !m_isJumping && m_hasLanded) {
    m_hasLanded = false;
    m_isJumping = true;
    m_headCollision = false;
    m_doJump = false;
  }

  FVector up = GetActorUpVector();
  FVector location = GetActorLocation();

  if (m_isJumping && !m_headCollision) {
    if (m_actualJumpSpeed > 0) {
      location += m_actualJumpSpeed * DeltaSeconds * up;

      m_actualJumpSpeed -= AccelerationJump * DeltaSeconds;
      if (m_actualJumpSpeed < 0)
        m_actualJumpSpeed = 0;
      m_enabledGravity = false;
      m_isFalling = false;
    }
    else {
      m_actualJumpSpeed = 0;
      m_isJumping = false;
    }
  }
  else {
    m_isFalling = !m_hasLanded;
    m_enabledGravity = true;
  }

  SetActorLocation(location);
}

void APlayerOvi::CalculateGravity(float DeltaSeconds){
  FVector up = GetActorUpVector();
  FVector location = GetActorLocation();

  if (m_enabledGravity && !m_hasLanded && !m_isOnMobilePlatform) {
    if (m_actualJumpSpeed < FallSpeed)
      m_actualJumpSpeed += AccelerationFall * DeltaSeconds;
    else{
      m_actualJumpSpeed = FallSpeed;
    }
    location -= m_actualJumpSpeed * DeltaSeconds * up;
    SetActorLocation(location);
  }
}

void APlayerOvi::CalculateOrientation(){
  FVector forward = GetActorForwardVector();
  FVector up = GetActorUpVector();

  float dotForward = FVector::DotProduct(GetActorLocation(), forward);

  if (dotForward > m_limit && m_state == States::RIGHT)
    Rotate(FVector(0, 0, -90));

  if (dotForward > m_limit && m_state == States::LEFT)
    Rotate(FVector(0, 0, 90));

  float dotUp = FVector::DotProduct(GetActorLocation(), up);
  float val = 0;

  if (dotUp > m_limit || dotUp < -m_limit) {
    bool toUp = dotUp > m_limit;

    val = (toUp) ? 90 : -90;

    if (m_state == States::RIGHT)
      Rotate(FVector(-val, 0, 0));
    else if (m_state == States::LEFT)
      Rotate(FVector(val, 0, 0));
  }

  AjustPosition();
}

void APlayerOvi::AjustPosition() {
  FVector location = GetActorLocation();

  if (location.X > m_limit)
    location.X = m_limit;
  else if (location.X < -m_limit)
    location.X = -m_limit;

  if (location.Y > m_limit)
    location.Y = m_limit;
  else if (location.Y < -m_limit)
    location.Y = -m_limit;

  if (location.Z > m_limit)
    location.Z = m_limit;
  else if (location.Z < -m_limit)
    location.Z = -m_limit;

  SetActorLocation(location);
}

void APlayerOvi::CheckCollision() {


  TArray<FHitResult> OutTraceResultTop;
  TArray<FHitResult> OutTraceResultBody;
  TArray<FHitResult> OutTraceResultMiddle;
  TArray<FHitResult> OutTraceResultLegs;
  TArray<FHitResult> OutTraceResultBottom;

  // Calculate the start location for trace  
  FVector StartTrace = m_lastPosition;
  FVector StartTraceTop = StartTrace + GetActorUpVector() * (m_capsuleHeight - m_capsuleHeightPadding);
  FVector StartTraceBottom = StartTrace - GetActorUpVector() * (m_capsuleHeight - m_capsuleHeightPadding);

  FVector StartTraceBody = StartTrace + GetActorUpVector() * (m_capsuleHeight - m_capsuleHeightPaddingFeet);
  FVector StartTraceLegs = StartTrace - GetActorUpVector() * (m_capsuleHeight - m_capsuleHeightPaddingFeet);

  FVector StartTraceLeftF = StartTrace + GetActorForwardVector() * (m_capsuleRadious - m_capsuleRadiousPadding);
  FVector StartTraceRigthF = StartTrace - GetActorForwardVector() * (m_capsuleRadious - m_capsuleRadiousPadding);

  // Calculate the end location for trace  
  //horizontal
  FVector newLocationForward;
  newLocationForward.X = (FMath::Abs(GetActorForwardVector().X) <= 0.01) ? m_lastPosition.X : GetActorLocation().X;
  newLocationForward.Y = (FMath::Abs(GetActorForwardVector().Y) <= 0.01) ? m_lastPosition.Y : GetActorLocation().Y;
  newLocationForward.Z = (FMath::Abs(GetActorForwardVector().Z) <= 0.01) ? m_lastPosition.Z : GetActorLocation().Z;

  const FVector EndTraceTop = (newLocationForward + GetActorUpVector() * (m_capsuleHeight - m_capsuleHeightPadding)) + GetActorForwardVector() * m_capsuleRadious;
  const FVector EndTraceBottom = (newLocationForward - GetActorUpVector() * (m_capsuleHeight - m_capsuleHeightPadding)) + GetActorForwardVector() * m_capsuleRadious;
  const FVector EndTraceBody = (newLocationForward + GetActorUpVector() * (m_capsuleHeight - m_capsuleHeightPaddingFeet)) + GetActorForwardVector() * m_capsuleRadious;
  const FVector EndTraceLegs = (newLocationForward - GetActorUpVector() * (m_capsuleHeight - m_capsuleHeightPaddingFeet)) + GetActorForwardVector() * m_capsuleRadious;
  const FVector EndTraceMidle = newLocationForward + GetActorForwardVector() * m_capsuleRadious;
 
  FCollisionResponseParams ResponseParam(ECollisionResponse::ECR_Overlap);

  GetWorld()->LineTraceMultiByChannel(OutTraceResultTop, StartTraceTop, EndTraceTop, COLLISION_PLAYER, m_TraceParams, ResponseParam);
  bool collisionTop = OutTraceResultTop.Num() > 0;
  //DrawDebugLine(GetWorld(), StartTraceTop, EndTraceTop, FColor(1.0f, 0.f, 0.f, 1.f), false, 10.f);
  GetWorld()->LineTraceMultiByChannel(OutTraceResultBottom, StartTraceBottom, EndTraceBottom, COLLISION_PLAYER, m_TraceParams, ResponseParam);
  bool collisionBottom = OutTraceResultBottom.Num() > 0;
  //DrawDebugLine(GetWorld(), StartTraceBottom, EndTraceBottom, FColor(1.0f, 0.f, 0.f, 1.f), false, 10.f);
  GetWorld()->LineTraceMultiByChannel(OutTraceResultBody, StartTraceBody, EndTraceBody, COLLISION_PLAYER, m_TraceParams, ResponseParam);
  bool collisionBody = OutTraceResultBody.Num() > 0;
  //DrawDebugLine(GetWorld(), StartTraceBody, EndTraceBody, FColor(1.0f, 0.f, 0.f, 1.f), false, 10.f);
  GetWorld()->LineTraceMultiByChannel(OutTraceResultLegs, StartTraceLegs, EndTraceLegs, COLLISION_PLAYER, m_TraceParams, ResponseParam);
  bool collisionLegs = OutTraceResultLegs.Num() > 0;
  //DrawDebugLine(GetWorld(), StartTraceLegs, EndTraceLegs, FColor(1.0f, 0.f, 0.f, 1.f), false, 10.f);

  GetWorld()->LineTraceMultiByChannel(OutTraceResultMiddle, StartTrace, EndTraceMidle, COLLISION_PLAYER, m_TraceParams, ResponseParam);
  bool collisionMidle = OutTraceResultMiddle.Num() > 0;
  //DrawDebugLine(GetWorld(), StartTrace, EndTraceMidle, FColor(1.0f, 0.f, 0.f, 1.f), false, 10.f);

  if (collisionTop || collisionBottom || collisionMidle) {

    if (collisionMidle) {
      int size = OutTraceResultMiddle.Num();
      for (int i = 0; i < size; i++)
        if (OutTraceResultMiddle[i].GetActor()->ActorHasTag("Platform")) {
        SetActorLocation(RecalculateLocation(GetActorForwardVector(), GetActorLocation(), OutTraceResultMiddle[i].Location, m_capsuleRadious));
        break;
        }
    }
    if (collisionTop) {
      int size = OutTraceResultTop.Num();
      for (int i = 0; i < size; i++)
        if (OutTraceResultTop[i].GetActor()->ActorHasTag("Platform")) {
        SetActorLocation(RecalculateLocation(GetActorForwardVector(), GetActorLocation(), OutTraceResultTop[i].Location, m_capsuleRadious));
        break;
        }
    }
    if (collisionBottom) {
      int size = OutTraceResultBottom.Num();
      for (int i = 0; i < size; i++)
        if (OutTraceResultBottom[i].GetActor()->ActorHasTag("Platform")) {
        SetActorLocation(RecalculateLocation(GetActorForwardVector(), GetActorLocation(), OutTraceResultBottom[i].Location, m_capsuleRadious));
        break;
        }
    }
  }
  else {
    TArray<FHitResult> OutTraceResultTopBack;
    TArray<FHitResult> OutTraceResultBodyBack;
    TArray<FHitResult> OutTraceResultMiddleBack;
    TArray<FHitResult> OutTraceResultBottomBack;
    TArray<FHitResult> OutTraceResultLegsBack;

    // Calculate the start location for trace back
    FVector StartTraceBack = newLocationForward;
    FVector StartTraceTopBack = StartTraceBack + GetActorUpVector() * (m_capsuleHeight - m_capsuleHeightPadding);
    FVector StartTraceBottomBack = StartTraceBack - GetActorUpVector() * (m_capsuleHeight - m_capsuleHeightPadding);

    FVector StartTraceBodyBack = StartTraceBack + GetActorUpVector() * (m_capsuleHeight - m_capsuleHeightPaddingFeet);
    FVector StartTraceLegsBack = StartTraceBack - GetActorUpVector() * (m_capsuleHeight - m_capsuleHeightPaddingFeet);

    //cambiado a last_position porque va hacia atras y la posicion nueva siempre sera hacia adelante.
    const FVector EndTraceTopBack = (m_lastPosition + GetActorUpVector() * (m_capsuleHeight - m_capsuleHeightPadding)) - GetActorForwardVector() * m_capsuleRadious;
    const FVector EndTraceBottomBack = (m_lastPosition - GetActorUpVector() * (m_capsuleHeight - m_capsuleHeightPadding)) - GetActorForwardVector() * m_capsuleRadious;
    const FVector EndTraceBodyBack = (m_lastPosition + GetActorUpVector() * (m_capsuleHeight - m_capsuleHeightPaddingFeet)) - GetActorForwardVector() * m_capsuleRadious;
    const FVector EndTraceLegsBack = (m_lastPosition - GetActorUpVector() * (m_capsuleHeight - m_capsuleHeightPaddingFeet)) - GetActorForwardVector() * m_capsuleRadious;
    const FVector EndTraceMidleBack = m_lastPosition - GetActorForwardVector() * m_capsuleRadious;

    GetWorld()->LineTraceMultiByChannel(OutTraceResultTopBack, StartTraceTopBack, EndTraceTopBack, COLLISION_PLAYER, m_TraceParams, ResponseParam);
    bool collisionTopBack = OutTraceResultTopBack.Num() > 0;
    //DrawDebugLine(GetWorld(), StartTraceTopBack, EndTraceTopBack, FColor(1.0f, 0.f, 0.f, 1.f), false, 10.f);
    GetWorld()->LineTraceMultiByChannel(OutTraceResultBottomBack, StartTraceBottomBack, EndTraceBottomBack, COLLISION_PLAYER, m_TraceParams, ResponseParam);
    bool collisionBottomBack = OutTraceResultBottomBack.Num() > 0;
    //DrawDebugLine(GetWorld(), StartTraceBottomBack, EndTraceBottomBack, FColor(1.0f, 0.f, 0.f, 1.f), false, 10.f);

    GetWorld()->LineTraceMultiByChannel(OutTraceResultBodyBack, StartTraceBodyBack, EndTraceBodyBack, COLLISION_PLAYER, m_TraceParams, ResponseParam);
    bool collisionBodyBack = OutTraceResultBodyBack.Num() > 0;
    //DrawDebugLine(GetWorld(), StartTraceBodyBack, EndTraceBodyBack, FColor(1.0f, 0.f, 0.f, 1.f), false, 10.f);
    GetWorld()->LineTraceMultiByChannel(OutTraceResultLegsBack, StartTraceLegsBack, EndTraceLegsBack, COLLISION_PLAYER, m_TraceParams, ResponseParam);
    bool collisionLegsBack = OutTraceResultLegsBack.Num() > 0;
    //DrawDebugLine(GetWorld(), StartTraceLegsBack, EndTraceLegsBack, FColor(1.0f, 0.f, 0.f, 1.f), false, 10.f);

    GetWorld()->LineTraceMultiByChannel(OutTraceResultMiddleBack, StartTraceBack, EndTraceMidleBack, COLLISION_PLAYER, m_TraceParams, ResponseParam);
    bool collisionMidleBack = OutTraceResultMiddleBack.Num() > 0;

    //DrawDebugLine(GetWorld(), StartTraceBack, EndTraceMidleBack, FColor(1.0f, 0.f, 0.f, 1.f), false, 10.f);
    if (collisionTopBack || collisionBottomBack || collisionMidleBack) {
      if (collisionMidleBack) {
        int size = OutTraceResultMiddleBack.Num();
        for (int i = 0; i < size; i++)
          if (OutTraceResultMiddleBack[i].GetActor()->ActorHasTag("MobilePlatform")) {
          if (!isPlayerRunning())
            SetActorLocation(RecalculateLocation(-GetActorForwardVector(), GetActorLocation(), OutTraceResultMiddleBack[i].Location, m_capsuleRadious));
          break;
          }
      }
      if (collisionTopBack) {
        if (collisionBodyBack) {
          int size = OutTraceResultTopBack.Num();
          for (int i = 0; i < size; i++)
            if (OutTraceResultTopBack[i].GetActor()->ActorHasTag("MobilePlatform")) {
            if (!isPlayerRunning())
              SetActorLocation(RecalculateLocation(-GetActorForwardVector(), GetActorLocation(), OutTraceResultTopBack[i].Location, m_capsuleRadious));
            break;
            }
        }
      }
      if (collisionBottomBack) {
        if (collisionLegsBack) {
          int size = OutTraceResultBottomBack.Num();
          for (int i = 0; i < size; i++)
            if (OutTraceResultBottomBack[i].GetActor()->ActorHasTag("MobilePlatform")) {
            if (!isPlayerRunning())
              SetActorLocation(RecalculateLocation(-GetActorForwardVector(), GetActorLocation(), OutTraceResultBottomBack[i].Location, m_capsuleRadious));
            break;
            }
        }
      }
    }
  }
  // Calculate the end location for trace  
  // Vertical 
  TArray<FHitResult> OutTraceResultDown;
  TArray<FHitResult> OutTraceResultDownLeftF;
  TArray<FHitResult> OutTraceResultDownRigthF;

  TArray<FHitResult> OutTraceResultUp;
  TArray<FHitResult> OutTraceResultUpLeftF;
  TArray<FHitResult> OutTraceResultUpRigthF;

  FVector newLocationUp;
  newLocationUp.X = (FMath::Abs(GetActorUpVector().X) <= 0.01) ? m_lastPosition.X : GetActorLocation().X;
  newLocationUp.Y = (FMath::Abs(GetActorUpVector().Y) <= 0.01) ? m_lastPosition.Y : GetActorLocation().Y;
  newLocationUp.Z = (FMath::Abs(GetActorUpVector().Z) <= 0.01) ? m_lastPosition.Z : GetActorLocation().Z;
  bool action = false;


  const FVector EndTraceDownLeftF = (newLocationUp + GetActorForwardVector() * (m_capsuleRadious - m_capsuleRadiousPadding)) - GetActorUpVector() * m_capsuleHeight;
  GetWorld()->LineTraceMultiByChannel(OutTraceResultDownLeftF, StartTraceLeftF, EndTraceDownLeftF, COLLISION_PLAYER, m_TraceParams, ResponseParam);
  bool collisionDownLeftF = OutTraceResultDownLeftF.Num() > 0;

  if (collisionDownLeftF) {
    if (collisionBottom == collisionLegs) {
      int size = OutTraceResultDownLeftF.Num();
      for (int i = 0; i < size; i++)
        if (OutTraceResultDownLeftF[i].GetActor()->ActorHasTag("Platform")) {
          if (OutTraceResultDownLeftF[i].GetActor()->ActorHasTag("MobilePlatform")) {
            AMobilePlatform *movil = dynamic_cast<AMobilePlatform *>(OutTraceResultDownLeftF[i].GetActor());
            if (movil) {
              if (m_currentMobile)
                if (m_currentMobile != movil)
                  m_currentMobile->SetPlayerOn(false);
              m_currentMobile = movil;
              m_currentMobile->SetPlayerOn(true);
            }
          }
          else {
            if (m_currentMobile) {
              m_currentMobile->SetPlayerOn(false);
              m_isOnMobilePlatform = false;
              m_currentMobile = nullptr;
            }
          }
        FVector localizaion = OutTraceResultDownLeftF[i].Location;
        action = true;
        SetActorLocation(RecalculateLocation(GetActorUpVector(), GetActorLocation(), OutTraceResultDownLeftF[i].Location, -m_capsuleHeight));
        m_hasLanded = true;
        m_isFalling = false;
        m_actualJumpSpeed = JumpSpeed;
        break;
        }
    }
  }
  newLocationUp.X = (FMath::Abs(GetActorUpVector().X) <= 0.01) ? m_lastPosition.X : GetActorLocation().X;
  newLocationUp.Y = (FMath::Abs(GetActorUpVector().Y) <= 0.01) ? m_lastPosition.Y : GetActorLocation().Y;
  newLocationUp.Z = (FMath::Abs(GetActorUpVector().Z) <= 0.01) ? m_lastPosition.Z : GetActorLocation().Z;
  const FVector EndTraceDownRightF = (newLocationUp - GetActorForwardVector() * (m_capsuleRadious - m_capsuleRadiousPadding)) - GetActorUpVector() * m_capsuleHeight;
  GetWorld()->LineTraceMultiByChannel(OutTraceResultDownRigthF, StartTraceRigthF, EndTraceDownRightF, COLLISION_PLAYER, m_TraceParams, ResponseParam);
  bool collisionDownRightF = OutTraceResultDownRigthF.Num() > 0;

  if (collisionDownRightF) {
    int size = OutTraceResultDownRigthF.Num();
    for (int i = 0; i < size; i++)
      if (OutTraceResultDownRigthF[i].GetActor()->ActorHasTag("Platform")) {
      if (OutTraceResultDownRigthF[i].GetActor()->ActorHasTag("MobilePlatform")) {
        AMobilePlatform *movil = dynamic_cast<AMobilePlatform *>(OutTraceResultDownRigthF[i].GetActor());
        if (movil) {
          if (m_currentMobile)
            if (m_currentMobile != movil)
              m_currentMobile->SetPlayerOn(false);
          m_currentMobile = movil;
          m_currentMobile->SetPlayerOn(true);
        }
      }
      else {
        if (m_currentMobile) {
          m_currentMobile->SetPlayerOn(false);
          m_isOnMobilePlatform = false;
          m_currentMobile = nullptr;
        }
      }
      action = true;
      FVector localizaion = OutTraceResultDownRigthF[i].Location;
      SetActorLocation(RecalculateLocation(GetActorUpVector(), GetActorLocation(), OutTraceResultDownRigthF[i].Location, -m_capsuleHeight));
      m_hasLanded = true;
      m_isFalling = false;
      m_actualJumpSpeed = JumpSpeed;
      break;
      }
  }

  if (!action)
  {
    if (m_currentMobile) {
      m_currentMobile->SetPlayerOn(false);
      m_isOnMobilePlatform = false;
      m_currentMobile = nullptr;
    }
    m_hasLanded = false;
  }

  newLocationUp.X = (FMath::Abs(GetActorUpVector().X) <= 0.01) ? m_lastPosition.X : GetActorLocation().X;
  newLocationUp.Y = (FMath::Abs(GetActorUpVector().Y) <= 0.01) ? m_lastPosition.Y : GetActorLocation().Y;
  newLocationUp.Z = (FMath::Abs(GetActorUpVector().Z) <= 0.01) ? m_lastPosition.Z : GetActorLocation().Z;

  //const FVector EndTraceUp = newLocationUp + GetActorUpVector() * m_capsuleHeight;
  const FVector EndTraceUpLeftF = (newLocationUp + GetActorForwardVector() * (m_capsuleRadious - m_capsuleRadiousPadding)) + GetActorUpVector() * m_capsuleHeight;
  const FVector EndTraceUpRightF = (newLocationUp - GetActorForwardVector() * (m_capsuleRadious - m_capsuleRadiousPadding)) + GetActorUpVector() * m_capsuleHeight;

  //GetWorld()->LineTraceMultiByChannel(OutTraceResultUp, StartTrace, EndTraceUp, COLLISION_PLAYER, m_TraceParams, ResponseParam);
  //DrawDebugLine(GetWorld(), StartTrace, EndTraceUp, FColor(1.0f, 0.f, 0.f, 1.f), false, 10.f);
  //bool collisionUp = OutTraceResultUp.Num() > 0;
  
  GetWorld()->LineTraceMultiByChannel(OutTraceResultUpLeftF, StartTraceLeftF, EndTraceUpLeftF, COLLISION_PLAYER, m_TraceParams, ResponseParam);
  //DrawDebugLine(GetWorld(), StartTraceLeftF, EndTraceUpLeftF, FColor(1.0f, 0.f, 0.f, 1.f), false, 10.f);
  bool collisionUpLeftF = OutTraceResultUpLeftF.Num() > 0;
  GetWorld()->LineTraceMultiByChannel(OutTraceResultUpRigthF, StartTraceRigthF, EndTraceUpRightF, COLLISION_PLAYER, m_TraceParams, ResponseParam);
  //DrawDebugLine(GetWorld(), StartTraceRigthF, EndTraceUpRightF, FColor(1.0f, 0.f, 0.f, 1.f), false, 10.f);
  bool collisionUpRightF = OutTraceResultUpRigthF.Num() > 0;

  if (collisionUpLeftF) {
      int size = OutTraceResultUpLeftF.Num();
      for (int i = 0; i < size; i++)
        if (OutTraceResultUpLeftF[i].GetActor()->ActorHasTag("Platform")) {
          if (OutTraceResultUpLeftF[i].GetActor()->ActorHasTag("MobilePlatform"))
            SetActorLocation(RecalculateLocation(GetActorUpVector(), GetActorLocation(), OutTraceResultUpLeftF[i].Location, m_capsuleHeight + 20));
          else
            SetActorLocation(RecalculateLocation(GetActorUpVector(), GetActorLocation(), OutTraceResultUpLeftF[i].Location, m_capsuleHeight));

          m_headCollision = true;
          m_actualJumpSpeed = 0.0f;
          break;
        }
  }
  else if (collisionUpRightF) {
    int size = OutTraceResultUpRigthF.Num();
    for (int i = 0; i < size; i++)
      if (OutTraceResultUpRigthF[i].GetActor()->ActorHasTag("Platform")) {
        if (OutTraceResultUpRigthF[i].GetActor()->ActorHasTag("MobilePlatform"))
          SetActorLocation(RecalculateLocation(GetActorUpVector(), GetActorLocation(), OutTraceResultUpRigthF[i].Location, m_capsuleHeight + 20));
        else
          SetActorLocation(RecalculateLocation(GetActorUpVector(), GetActorLocation(), OutTraceResultUpRigthF[i].Location, m_capsuleHeight));

        m_headCollision = true;
        m_actualJumpSpeed = 0.0f;
        break;
      }
  }
  
}

void APlayerOvi::Rotate(const FVector& rotation) {
  FTransform transform = GetTransform();
  FQuat q = transform.GetRotation() * FQuat::MakeFromEuler(rotation);
  transform.SetRotation(q);
  SetActorTransform(transform);
}

FVector APlayerOvi::AbsVector(const FVector& vector) {
  FVector absVector = FVector::ZeroVector;

  absVector.X = FMath::Abs(vector.X);
  absVector.Y = FMath::Abs(vector.Y);
  absVector.Z = FMath::Abs(vector.Z);

  return absVector;
}

FVector APlayerOvi::RecalculateLocation(FVector Direction, FVector Location, FVector HitLocation, float size)
{
  FVector loc = Location;
  FVector absDirection = FVector::ZeroVector;
  FVector dir = Direction;

  absDirection.X = (FMath::Abs(dir.X) <= 0.01) ? 0 : 1;
  absDirection.Y = (FMath::Abs(dir.Y) <= 0.01) ? 0 : 1;
  absDirection.Z = (FMath::Abs(dir.Z) <= 0.01) ? 0 : 1;

  FVector a = (absDirection * HitLocation) - (Direction * size);
  FVector nPos = (absDirection * a);

  loc.X = (FMath::Abs(nPos.X) <= 0.01) ? loc.X : nPos.X;
  loc.Y = (FMath::Abs(nPos.Y) <= 0.01) ? loc.Y : nPos.Y;
  loc.Z = (FMath::Abs(nPos.Z) <= 0.01) ? loc.Z : nPos.Z;

  return loc;
}

void APlayerOvi::OnMobilePlatform(AMobilePlatform *mp, FVector movement){
  if (!m_isJumping){
    FVector loc = GetActorLocation();
    SetActorLocation(loc + movement);
    m_isOnMobilePlatform = true;
  }
  else{
    m_isOnMobilePlatform = false;
  }
}

void APlayerOvi::SetKey(bool key, FLinearColor colorKey) {
  m_hasKey = key;
  m_colorKey = colorKey;
  m_stick->SetColor(m_colorKey, 5.0f);
}

bool APlayerOvi::HasKey(){
  return m_hasKey;
}

FLinearColor APlayerOvi::GetColorKey(){
  return m_colorKey;
}

bool APlayerOvi::isPlayerRunning() {
  return bPlayerRunning;
}

bool APlayerOvi::PlayerStopRunning() {
  return !bPlayerRunning;
}

bool APlayerOvi::isPlayerJumping() {
  return m_isJumping;
}

bool APlayerOvi::isPlayerFalling() {
  return m_isFalling;
}

bool APlayerOvi::PlayerHasLanded() {
  return m_hasLanded;
}

bool APlayerOvi::PlayerisToRight() {
  return (m_state == States::RIGHT);
}

bool APlayerOvi::isPlayerPaused() {
  return TimeManager::Instance()->IsPaused();
}

bool APlayerOvi::isInputEnabled() {
  float gameStatus = m_gameMode->EndGameBP();
  return (gameStatus < 0.05f && gameStatus > -0.05f) && !m_isPushingButton && !m_isPickingPortal && !m_isPickingAltar && !m_inTutorial;
}

bool APlayerOvi::PlayerisPushinButton() {
  return m_isPushingButton;
}

bool APlayerOvi::PlayerisPickingPortal() {
  return m_isPickingPortal;
}

bool APlayerOvi::PlayerisPickingAltar() {
  return m_isPickingAltar;
}

void  APlayerOvi::EnabledPushButton() {
  m_isPushingButton = true;
}

void  APlayerOvi::EnabledPickPortal() {
  m_isPickingPortal = true;
}

void  APlayerOvi::EnabledPickAltar() {
  m_isPickingAltar = true;
}

void APlayerOvi::inTutorial(bool value){
  m_inTutorial = value;
  if (value)
    bPlayerRunning = false;
}

void APlayerOvi::ResetToCheckPoint(FTransform playerTransform, bool right) {
  SetActorTransform(playerTransform);
  bPlayerRunning = false;
  m_state = (right) ? States::RIGHT : States::LEFT;
  m_isPickingAltar = m_isPickingPortal = m_isPushingButton = false;
  m_inTutorial = false;
}

void APlayerOvi::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  if (m_stick)
    GetWorld()->DestroyActor(m_stick);

  m_stick = nullptr;
  CapsuleComponent = nullptr;
  Mesh = nullptr;
  m_currentMobile = nullptr;
  m_gameMode = nullptr;
}