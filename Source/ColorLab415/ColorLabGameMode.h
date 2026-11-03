#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/HUD.h"
#include "ColorLabGameMode.generated.h"
class UCameraComponent;
UCLASS()
class COLORLAB415_API AColorLabCharacter : public ACharacter
{
 GENERATED_BODY()
public:
 AColorLabCharacter();
 virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
 virtual void Tick(float Delta) override;
 virtual void BeginPlay() override;
 UPROPERTY(VisibleAnywhere) TObjectPtr<UCameraComponent> Camera;
private:
 void Forward(float Value); void Right(float Value); void Fire(); void ToggleDemo();
 bool bDemo=false; float ShotTimer=0.f; int32 ShotNumber=0;
 bool bCapture=false; int32 CaptureFrame=0; float CaptureTimer=0.f; float CaptureElapsed=0.f;
};
UCLASS()
class COLORLAB415_API AColorLabHUD : public AHUD
{
 GENERATED_BODY()
public: virtual void DrawHUD() override;
};
UCLASS()
class COLORLAB415_API AColorLabGameMode : public AGameModeBase
{
 GENERATED_BODY()
public: AColorLabGameMode();
};
