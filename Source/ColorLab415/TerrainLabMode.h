#pragma once
#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/HUD.h"
#include "TerrainLabMode.generated.h"
class AProceduralTerrain;
// A flying inspection pawn keeps both the tutorial meshes and terrain easy to inspect.
UCLASS()
class COLORLAB415_API ATerrainLabPawn : public ADefaultPawn
{
 GENERATED_BODY()
public:
 ATerrainLabPawn();
 virtual void BeginPlay() override;
 virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
 virtual void Tick(float Delta) override;
 void Sculpt(float Direction);
 void Dig(){Sculpt(-1);} void Raise(){Sculpt(1);} void ResetTerrain();
 UPROPERTY() TObjectPtr<AProceduralTerrain> Terrain;
 FString LastAction=TEXT("Aim at terrain and click to dig");
private:
 bool bCapture=false;float Elapsed=0,FrameTimer=0,EditTimer=0;int32 Frame=0,DemoStep=0;
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
