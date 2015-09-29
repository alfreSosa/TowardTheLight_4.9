#pragma once

#include "GameFramework/Actor.h"
#include "Tutorial.generated.h"

class ATowardsTheLightGameMode;

UCLASS()
class TOWARDSTHELIGHT_API ATutorial : public AActor {
	GENERATED_BODY()
	
public:	
  //properties
  UPROPERTY(EditAnywhere)
    UBoxComponent *TriggerIn;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TutorialText)
    FString Key;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TutorialInit)
    bool AutoLoad;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TutorialTime)
    float TimeAfterOut;
  //functions
  ATutorial();
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaSeconds) override;
  void RegisterDelegate();
  void EndPlay(const EEndPlayReason::Type EndPlayReason);

  void RestoreInitialState();

  //Delegates
  UFUNCTION()
    void OnBeginTriggerOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
  //UFUNCTION()
  //  void OnTriggerOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:
  ATowardsTheLightGameMode *m_gameMode;
  //properties
  //bool m_loaded;
  bool m_enter;
  //bool m_end;
  //float m_currentTime;
};
