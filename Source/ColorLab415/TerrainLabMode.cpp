#include "TerrainLabMode.h"
#include "ProceduralTerrain.h"
#include "Components/InputComponent.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "UnrealClient.h"

ATerrainLabPawn::ATerrainLabPawn(){PrimaryActorTick.bCanEverTick=true;}
void ATerrainLabPawn::BeginPlay()
{
 Super::BeginPlay();
 for(TActorIterator<AProceduralTerrain> It(GetWorld());It;++It)
  if(It->Exercise==EMeshExercise::Terrain){Terrain=*It;break;}
 SetActorLocation(FVector(-800,-1100,1600));
 if(Controller)Controller->SetControlRotation((FVector(1700,1300,0)-GetActorLocation()).Rotation());
 bCapture=FParse::Param(FCommandLine::Get(),TEXT("TerrainCapture"));
 if(bCapture)IFileManager::Get().MakeDirectory(*(FPaths::ProjectSavedDir()/TEXT("CaptureWeek4")),true);
}
void ATerrainLabPawn::SetupPlayerInputComponent(UInputComponent* Input)
{
 Super::SetupPlayerInputComponent(Input);
 Input->BindAction(TEXT("Fire"),IE_Pressed,this,&ATerrainLabPawn::Dig);
 Input->BindKey(EKeys::RightMouseButton,IE_Pressed,this,&ATerrainLabPawn::Raise);
 Input->BindKey(EKeys::R,IE_Pressed,this,&ATerrainLabPawn::ResetTerrain);
}
void ATerrainLabPawn::Sculpt(float Direction)
{
 // Trace from the player's actual view, not from a guessed terrain coordinate.
 FVector Start;FRotator Rotation;GetActorEyesViewPoint(Start,Rotation);
 FHitResult Hit;FCollisionQueryParams Query(SCENE_QUERY_STAT(TerrainBrush),true,this);
 const bool bHit=GetWorld()->LineTraceSingleByChannel(Hit,Start,Start+Rotation.Vector()*12000,ECC_Visibility,Query);
 AProceduralTerrain* Target=bHit?Cast<AProceduralTerrain>(Hit.GetActor()):nullptr;
 if(Target && Target->AlterAt(Hit.ImpactPoint,Direction))
 {
  LastAction=Direction<0?TEXT("DIG: vertices lowered; collision updated"):TEXT("RAISE: vertices lifted; collision updated");
  DrawDebugSphere(GetWorld(),Hit.ImpactPoint,Target->Radius,32,FColor::Orange,false,1.2f);
  UE_LOG(LogTemp,Display,TEXT("PLAYER_TRACE_HIT point=%s direction=%.0f"),*Hit.ImpactPoint.ToString(),Direction);
 }
 else LastAction=TEXT("No editable terrain under crosshair");
}
void ATerrainLabPawn::ResetTerrain(){if(Terrain){Terrain->Rebuild();LastAction=TEXT("Original Perlin terrain restored");}}
void ATerrainLabPawn::Tick(float Delta)
{
 Super::Tick(Delta);if(!bCapture)return;Elapsed+=Delta;FrameTimer-=Delta;EditTimer-=Delta;
 // Demonstration calls the same view ray trace as manual input; only camera aiming is automated.
 if(Elapsed>4 && Elapsed<17 && EditTimer<=0 && Terrain)
 {
  EditTimer=.7f;const bool bRaise=Elapsed>11;
  const FVector Aim=bRaise?FVector(2300,1500,0):FVector(1300,1100,0);
  if(Controller)Controller->SetControlRotation((Aim-GetActorLocation()).Rotation());
  Sculpt(bRaise?1.f:-1.f);
 }
 if(Elapsed>18 && DemoStep==0){ResetTerrain();++DemoStep;}
 if(Elapsed>21 && DemoStep==1 && Terrain){Terrain->XSize=40;Terrain->YSize=32;Terrain->Rebuild();LastAction=TEXT("Resized to 40 x 32 cells");++DemoStep;}
 if(Elapsed>24 && DemoStep==2 && Terrain){Terrain->XSize=64;Terrain->YSize=48;Terrain->Rebuild();LastAction=TEXT("Restored to 64 x 48 cells");++DemoStep;}
 if(Elapsed>2 && FrameTimer<=0){FrameTimer=.1f;FScreenshotRequest::RequestScreenshot(FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()/FString::Printf(TEXT("CaptureWeek4/frame%04d.png"),Frame++)),true,false);}
 if(Elapsed>27){bCapture=false;UE_LOG(LogTemp,Display,TEXT("TERRAIN_CAPTURE_COMPLETE frames=%d"),Frame);}
}
void ATerrainLabHUD::DrawHUD()
{
 Super::DrawHUD();if(!Canvas)return;
 DrawRect(FLinearColor(.015,.025,.04,.9),18,18,1000,116);
 DrawText(TEXT("TERRAIN LAB | GAM 415 - WEEK 4"),FLinearColor::White,32,26,nullptr,1.4);
 DrawText(TEXT("WASD fly | Mouse aim | Left click dig | Right click raise | R reset | E/Q up/down"),FLinearColor(.65,.85,1),32,60);
 if(auto* P=Cast<ATerrainLabPawn>(GetOwningPawn()))
 {
  DrawText(P->LastAction,FLinearColor(1,.75,.3),32,82);
  if(P->Terrain)DrawText(FString::Printf(TEXT("Perlin grid %d x %d | %d vertices | %d triangles | edits %d | Blue plane + cube: tutorial"),P->Terrain->XSize,P->Terrain->YSize,P->Terrain->VertexCount(),P->Terrain->TriangleCount(),P->Terrain->EditCount),FLinearColor::White,32,104);
 }
 DrawLine(Canvas->ClipX/2-8,Canvas->ClipY/2,Canvas->ClipX/2+8,Canvas->ClipY/2,FLinearColor::White);
 DrawLine(Canvas->ClipX/2,Canvas->ClipY/2-8,Canvas->ClipX/2,Canvas->ClipY/2+8,FLinearColor::White);
}
ATerrainLabMode::ATerrainLabMode(){DefaultPawnClass=ATerrainLabPawn::StaticClass();HUDClass=ATerrainLabHUD::StaticClass();}
