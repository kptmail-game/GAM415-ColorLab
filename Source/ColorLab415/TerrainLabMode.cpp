#include "TerrainLabMode.h"
#include "ProceduralTerrain.h"
#include "ColorProjectile.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"

ATerrainLabPawn::ATerrainLabPawn()
{
 PrimaryActorTick.bCanEverTick=true;
 Camera=CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
 Camera->SetupAttachment(RootComponent);
 Camera->SetRelativeLocation(FVector(0,0,64)); Camera->bUsePawnControlRotation=true;
 Camera->SetFieldOfView(85);
 GetCharacterMovement()->MaxWalkSpeed=450;
 static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
 static ConstructorHelpers::FObjectFinder<UStaticMesh> Cylinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
 static ConstructorHelpers::FObjectFinder<UMaterialInterface> Metal(TEXT("/Game/Week4/M_Rifle.M_Rifle"));
 // Original rifle assembled from engine meshes; every part follows the camera.
 auto Part=[&](const TCHAR* Name,UStaticMesh* Shape,FVector Pos,FVector Scale,FRotator Rot=FRotator::ZeroRotator)
 {
  auto* Mesh=CreateDefaultSubobject<UStaticMeshComponent>(Name);
  Mesh->SetupAttachment(Camera);Mesh->SetStaticMesh(Shape);
  Mesh->SetRelativeLocation(Pos);Mesh->SetRelativeScale3D(Scale);Mesh->SetRelativeRotation(Rot);
  Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);Mesh->SetCastShadow(false);
  if(Metal.Succeeded())Mesh->SetMaterial(0,Metal.Object);
 };
 Part(TEXT("RifleReceiver"),Cube.Object,FVector(43,18,-17),FVector(.40,.10,.12));
 Part(TEXT("RifleStock"),Cube.Object,FVector(15,18,-20),FVector(.22,.09,.18));
 Part(TEXT("RifleBarrel"),Cylinder.Object,FVector(73,18,-14),FVector(.045,.045,.36),FRotator(90,0,0));
 Part(TEXT("RifleGrip"),Cube.Object,FVector(35,18,-29),FVector(.09,.08,.20),FRotator(-15,0,0));
 Part(TEXT("RifleMagazine"),Cube.Object,FVector(51,18,-29),FVector(.12,.08,.20),FRotator(12,0,0));
 Part(TEXT("RifleSight"),Cube.Object,FVector(48,18,-8),FVector(.12,.04,.06));
 Muzzle=CreateDefaultSubobject<USceneComponent>(TEXT("RifleMuzzle"));
 Muzzle->SetupAttachment(Camera);Muzzle->SetRelativeLocation(FVector(94,18,-14));
}
void ATerrainLabPawn::BeginPlay()
{
 Super::BeginPlay();
 for(TActorIterator<AProceduralTerrain> It(GetWorld());It;++It)
  if(It->Exercise==EMeshExercise::Terrain){Terrain=*It;break;}
 SetActorLocation(FVector(-350,1400,450));
 if(Controller)Controller->SetControlRotation(FRotator(-18,0,0));
}
void ATerrainLabPawn::SetupPlayerInputComponent(UInputComponent* Input)
{
 Super::SetupPlayerInputComponent(Input);
 Input->BindAxis(TEXT("Forward"),this,&ATerrainLabPawn::Forward);
 Input->BindAxis(TEXT("Right"),this,&ATerrainLabPawn::Right);
 Input->BindAxis(TEXT("Turn"),this,&ACharacter::AddControllerYawInput);
 Input->BindAxis(TEXT("Look"),this,&ACharacter::AddControllerPitchInput);
 Input->BindAction(TEXT("Jump"),IE_Pressed,this,&ACharacter::Jump);
 Input->BindAction(TEXT("Fire"),IE_Pressed,this,&ATerrainLabPawn::Dig);
 Input->BindKey(EKeys::RightMouseButton,IE_Pressed,this,&ATerrainLabPawn::Raise);
 Input->BindKey(EKeys::R,IE_Pressed,this,&ATerrainLabPawn::ResetTerrain);
 Input->BindAction(TEXT("Demo"),IE_Pressed,this,&ATerrainLabPawn::ToggleDemo);
}
void ATerrainLabPawn::Forward(float V){AddMovementInput(GetActorForwardVector(),V);}
void ATerrainLabPawn::Right(float V){AddMovementInput(GetActorRightVector(),V);}
void ATerrainLabPawn::FireWeapon(float Direction)
{
 // View ray selects an aim point ONLY. Travelling projectiles must collide to sculpt.
 const FVector Start=Camera->GetComponentLocation();
 const FVector End=Start+Camera->GetForwardVector()*12000;
 FHitResult AimHit;FCollisionQueryParams Query(SCENE_QUERY_STAT(RifleAim),true,this);
 const bool bHit=GetWorld()->LineTraceSingleByChannel(AimHit,Start,End,ECC_Visibility,Query);
 const FVector AimPoint=bHit?AimHit.ImpactPoint:End;
 const FVector Origin=Muzzle->GetComponentLocation();
 const FTransform SpawnTransform((AimPoint-Origin).Rotation(),Origin);
 // Deferred spawning sets edit direction before BeginPlay; no edit happens here.
 auto* Shot=GetWorld()->SpawnActorDeferred<AColorProjectile>(AColorProjectile::StaticClass(),SpawnTransform,this,this,ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
 if(Shot)
 {
  Shot->TerrainDirection=Direction;Shot->FinishSpawning(SpawnTransform);++ShotsFired;
  LastAction=Direction<0?TEXT("Rifle fired DIG round; waiting for physical impact"):TEXT("Rifle fired RAISE round; waiting for physical impact");
  UE_LOG(LogTemp,Display,TEXT("RIFLE_FIRE shot=%d muzzle=%s terrainEditsAtFire=%d"),ShotsFired,*Origin.ToString(),Terrain?Terrain->EditCount:-1);
 }
}
void ATerrainLabPawn::ResetTerrain()
{
 bDemo=false;
 // Remove in-flight rounds so an old shot cannot modify the reset surface.
 for(TActorIterator<AColorProjectile> It(GetWorld());It;++It)if(It->GetOwner()==this)It->Destroy();
 if(Terrain)Terrain->Rebuild();ShotsFired=0;LastAction=TEXT("Perlin terrain reset; no in-flight rounds");
}
void ATerrainLabPawn::ToggleDemo(){bDemo=!bDemo;ShotTimer=0;}
void ATerrainLabPawn::Tick(float Delta)
{
 Super::Tick(Delta);
 // F uses the same rifle/muzzle/projectile/impact path as manual clicking.
 if(bDemo){ShotTimer-=Delta;if(ShotTimer<=0){ShotTimer=.65f;Dig();}}
}
void ATerrainLabHUD::DrawHUD()
{
 Super::DrawHUD();if(!Canvas)return;
 DrawRect(FLinearColor(.015,.025,.04,.90),12,12,790,98);
 DrawText(TEXT("PROCEDURAL TERRAIN | RIFLE PROJECTILE IMPACTS"),FLinearColor::White,24,20,nullptr,1.1);
 DrawText(TEXT("WASD move | Mouse aim | Click dig round | Right click raise round | R reset | F repeat fire"),FLinearColor(.65,.85,1),24,46);
 if(auto* P=Cast<ATerrainLabPawn>(GetOwningPawn()))
 {
  DrawText(P->LastAction,FLinearColor(1,.75,.3),24,66);
  if(P->Terrain)DrawText(FString::Printf(TEXT("Perlin %d x %d | vertices %d | shots %d | terrain impacts %d"),P->Terrain->XSize,P->Terrain->YSize,P->Terrain->VertexCount(),P->ShotsFired,P->Terrain->EditCount),FLinearColor::White,24,86);
 }
 DrawLine(Canvas->ClipX/2-7,Canvas->ClipY/2,Canvas->ClipX/2+7,Canvas->ClipY/2,FLinearColor::White);
 DrawLine(Canvas->ClipX/2,Canvas->ClipY/2-7,Canvas->ClipX/2,Canvas->ClipY/2+7,FLinearColor::White);
}
ATerrainLabMode::ATerrainLabMode(){DefaultPawnClass=ATerrainLabPawn::StaticClass();HUDClass=ATerrainLabHUD::StaticClass();}
