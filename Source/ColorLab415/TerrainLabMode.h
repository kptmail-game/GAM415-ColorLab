#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/HUD.h"
#include "TerrainLabMode.generated.h"
class AProceduralTerrain;
class UCameraComponent;
class USceneComponent;
// Grounded first-person rifle character. Aiming never edits terrain directly.
UCLASS()
class COLORLAB415_API ATerrainLabPawn : public ACharacter
{
 GENERATED_BODY()
public:
 ATerrainLabPawn();
 virtual void BeginPlay() override;
 virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
 virtual void Tick(float Delta) override;
 UPROPERTY(VisibleAnywhere) TObjectPtr<UCameraComponent> Camera;
 UPROPERTY(VisibleAnywhere) TObjectPtr<USceneComponent> Muzzle;
 UPROPERTY() TObjectPtr<AProceduralTerrain> Terrain;
 FString LastAction=TEXT("Left click fires a digging projectile");
 int32 ShotsFired=0;
 void ResetTerrain();
private:
 void Forward(float Value); void Right(float Value);
 void FireWeapon(float Direction);
 void Dig(){FireWeapon(-1.f);} void Raise(){FireWeapon(1.f);}
 void ToggleDemo();
 bool bDemo=false;
 float ShotTimer=0;
};
UCLASS()
class COLORLAB415_API ATerrainLabHUD : public AHUD
{
 GENERATED_BODY()
public: virtual void DrawHUD() override;
};
UCLASS()
class COLORLAB415_API ATerrainLabMode : public AGameModeBase
{
 GENERATED_BODY()
public: ATerrainLabMode();
};
