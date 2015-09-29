#pragma once

#include "GameFramework/Pawn.h"
#include "PlayerOvi.generated.h"

#define COLLISION_PLAYER        ECC_GameTraceChannel1 

class AMobilePlatform;
class AStick;
class ATowardsTheLightGameMode;

UCLASS()
class TOWARDSTHELIGHT_API APlayerOvi : public APawn
{
	GENERATED_BODY()

public:
	enum States { RIGHT, LEFT, STOP};
  //public functions
	APlayerOvi();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
  void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
  void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);
  void TouchEnd(const ETouchIndex::Type FingerIndex, const FVector Location);
  void OnMobilePlatform(class AMobilePlatform *mp, FVector movement);
  void SetKey(bool key, FLinearColor colorKey);
  bool HasKey();
  FLinearColor GetColorKey();
  void EnabledPushButton();
  void EnabledPickPortal();
  void EnabledPickAltar();
  void ResetToCheckPoint(FTransform playerTransform, bool right);

  //public editor functions
  UFUNCTION(BlueprintCallable, Category = "PlayerInputTTL")
		void OnStartRight();
  UFUNCTION(BlueprintCallable, Category = "PlayerInputTTL")
		void OnStopRight();
  UFUNCTION(BlueprintCallable, Category = "PlayerInputTTL")
		void OnStartLeft();
  UFUNCTION(BlueprintCallable, Category = "PlayerInputTTL")
		void OnStopLeft();
  UFUNCTION(BlueprintCallable, Category = "PlayerInputTTL")
    void OnStartJump();
  UFUNCTION(BlueprintCallable, Category = "PlayerInputTTL")
    void OnStopJump();

  //public Animations functions
  UFUNCTION(BlueprintCallable, Category = "PlayerLocomotion")
    bool isPlayerRunning();
  UFUNCTION(BlueprintCallable, Category = "PlayerLocomotion")
    bool PlayerStopRunning();
  UFUNCTION(BlueprintCallable, Category = "PlayerLocomotion")
    bool isPlayerJumping();
  UFUNCTION(BlueprintCallable, Category = "PlayerLocomotion")
    bool isPlayerFalling();
  UFUNCTION(BlueprintCallable, Category = "PlayerLocomotion")
    bool PlayerHasLanded();
  UFUNCTION(BlueprintCallable, Category = "PlayerLocomotion")
    bool PlayerisToRight();
  UFUNCTION(BlueprintCallable, Category = "PlayerLocomotion")
    bool PlayerisPushinButton();
  UFUNCTION(BlueprintCallable, Category = "PlayerLocomotion")
    bool PlayerisPickingPortal();
  UFUNCTION(BlueprintCallable, Category = "PlayerLocomotion")
    bool PlayerisPickingAltar();
  UFUNCTION(BlueprintCallable, Category = "PlayerState")
    bool isPlayerPaused();

  UFUNCTION(BlueprintCallable, Category = "PlayerInputTTL")
    void inTutorial(bool value);

  //public editor propierties
  UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	  class UCapsuleComponent* CapsuleComponent;
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	  class USkeletalMeshComponent* Mesh;
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	  class UArrowComponent* ArrowComponent;
#endif
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputControl)
    float MarginInput;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float MovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float JumpSpeed;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
    float FallSpeed;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
    float AccelerationJump;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
    float AccelerationFall;
private:

  bool m_isValid;
  //private functions  
  float UpdateState();
  void CalculateOrientation();
  void CalculateGravity(float DeltaSeconds);
  void DoJump(float DeltaSeconds);
  void DoMovement(float DeltaSeconds, float value);
  void CheckCollision();
  void AjustPosition();
  void Rotate(const FVector& rotation);
  bool isInputEnabled();
  FVector AbsVector(const FVector& vector);
  FVector RecalculateLocation(FVector Direction, FVector Location, FVector HitLocation, float size);
  
  //private propiertes
  ATowardsTheLightGameMode *m_gameMode;
  
  //player movement propiertes
  FCollisionQueryParams m_TraceParams; 
  //movement state
  States m_state;
	bool m_right;
	bool m_left;
  //jump state
  bool m_doJump;
  bool m_isJumping;
  bool m_isFalling;
  bool m_enabledGravity;
  //collisions
  bool m_hasLanded;
  bool m_headCollision;
  float m_limit; //limit cube face

  //jump velocity
  float m_actualAccJump;
  float m_actualJumpSpeed;

  //collision distances
  float m_capsuleHeight;
  float m_capsuleRadious;
  float m_capsuleHeightPadding;
  float m_capsuleHeightPaddingFeet;
  float m_capsuleRadiousPadding;

  //origin ray position
  FVector m_lastPosition;
  //mobile platform interaction propierties
  bool m_isOnMobilePlatform;
  AMobilePlatform *m_currentMobile;

  //animation boton
  bool m_isPushingButton;
  bool m_isPickingAltar;
  bool m_isPickingPortal;
  float m_elapsedButton;
  float m_elapsedPortal;
  float m_elapsedAltar;

  //tutorial
  bool m_inTutorial;

  //key & stick
  bool m_hasKey;
  FLinearColor m_colorKey;
  AStick* m_stick;

  //animation propiertie
  bool bPlayerRunning;

  //input propierties
  float m_limitViewPort0;
  float m_limitViewPort1;
  float m_centerTouchX;
  States m_stateInput;
  ETouchIndex::Type m_fingerIndexMovement;
  ETouchIndex::Type m_fingerIndexJump;
  ETouchIndex::Type m_fingerIndexOther;
};
