#include "ColorLabGameMode.h"
#include "ColorProjectile.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "UnrealClient.h"
void AColorLabCharacter::BeginPlay()
{
 Super::BeginPlay();
 // Optional artifact capture records actual rendered game frames for the walkthrough.
 bCapture=FParse::Param(FCommandLine::Get(),TEXT("LabCapture"));
 if(bCapture){bDemo=true;SetActorLocation(FVector(250,0,100));IFileManager::Get().MakeDirectory(*(FPaths::ProjectSavedDir()/TEXT("CaptureWeek3")),true);}
}
AColorLabCharacter::AColorLabCharacter()
{
 PrimaryActorTick.bCanEverTick=true;
 Camera=CreateDefaultSubobject<UCameraComponent>(TEXT("Camera")); Camera->SetupAttachment(RootComponent);
 Camera->SetRelativeLocation(FVector(0,0,64)); Camera->bUsePawnControlRotation=true;
 GetCharacterMovement()->MaxWalkSpeed=450;
}
void AColorLabCharacter::SetupPlayerInputComponent(UInputComponent* Input)
{
 Super::SetupPlayerInputComponent(Input);
 Input->BindAxis(TEXT("Forward"),this,&AColorLabCharacter::Forward);
 Input->BindAxis(TEXT("Right"),this,&AColorLabCharacter::Right);
 Input->BindAxis(TEXT("Turn"),this,&ACharacter::AddControllerYawInput);
 Input->BindAxis(TEXT("Look"),this,&ACharacter::AddControllerPitchInput);
 Input->BindAction(TEXT("Fire"),IE_Pressed,this,&AColorLabCharacter::Fire);
 Input->BindAction(TEXT("Demo"),IE_Pressed,this,&AColorLabCharacter::ToggleDemo);
 Input->BindAction(TEXT("Jump"),IE_Pressed,this,&ACharacter::Jump);
}
void AColorLabCharacter::Forward(float V){AddMovementInput(GetActorForwardVector(),V);}
void AColorLabCharacter::Right(float V){AddMovementInput(GetActorRightVector(),V);}
void AColorLabCharacter::Fire()
{
 FActorSpawnParameters Params; Params.Owner=this; Params.Instigator=this;
 Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
 const FRotator Aim=Camera->GetComponentRotation();
 GetWorld()->SpawnActor<AColorProjectile>(Camera->GetComponentLocation()+Aim.Vector()*70.f,Aim,Params);
}
void AColorLabCharacter::ToggleDemo(){bDemo=!bDemo;ShotTimer=0;}
void AColorLabCharacter::Tick(float Delta)
{
 Super::Tick(Delta);
 if(bCapture)
 {
  CaptureElapsed+=Delta;CaptureTimer-=Delta;
  if(CaptureElapsed>2.f && CaptureTimer<=0.f)
  {
   CaptureTimer=.05f;
   FScreenshotRequest::RequestScreenshot(FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()/FString::Printf(TEXT("CaptureWeek3/frame%04d.png"),CaptureFrame++)),true,false);
  }
  // Capture the dust before entry, then enter and leave twice to test live recoloring.
  if(CaptureElapsed>17.f && CaptureElapsed<19.f){bDemo=false;SetActorLocation(FVector(-150,0,100));if(Controller)Controller->SetControlRotation(FRotator(0,-90,0));}
  if(CaptureElapsed>19.f && CaptureElapsed<21.f)SetActorLocation(FVector(-150,-300,100));
  if(CaptureElapsed>21.f && CaptureElapsed<23.f)SetActorLocation(FVector(-150,0,100));
  if(CaptureElapsed>23.f && CaptureElapsed<25.f)SetActorLocation(FVector(-150,-300,100));
  if(CaptureElapsed>27.f){bCapture=false;bDemo=false;}
 }
 // Optional repeatable demonstration uses the same projectile and hit path as manual fire.
 if(!bDemo)return;
 ShotTimer-=Delta;
 if(ShotTimer<=0)
 {
  ShotTimer=.8f;
  const FVector Target(1000.f,-420.f+(ShotNumber%7)*140.f,100.f+((ShotNumber/7)%3)*130.f);
  const FRotator Aim=(Target-Camera->GetComponentLocation()).Rotation();
  FActorSpawnParameters P;P.Owner=this;P.Instigator=this;
  P.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
  GetWorld()->SpawnActor<AColorProjectile>(Camera->GetComponentLocation()+Aim.Vector()*70.f,Aim,P);
  ++ShotNumber;
 }
}
void AColorLabHUD::DrawHUD()
{
 Super::DrawHUD();if(!Canvas)return;
 DrawRect(FLinearColor(0.015f,.025f,.04f,.85f),18,18,760,84);
 DrawText(TEXT("PARTICLE LAB | GAM 415 - WEEK 3"),FLinearColor::White,32,28,nullptr,1.4f);
 DrawText(TEXT("WASD move | Mouse aim | Click fire | F demo | Space jump"),FLinearColor(.6f,.85f,1.f),32,62);
 DrawText(TEXT("Paint bursts match each shot | Walk into floating dust to recolor"),FLinearColor(.8f,.8f,.8f),32,82);
 DrawLine(Canvas->ClipX/2-7,Canvas->ClipY/2,Canvas->ClipX/2+7,Canvas->ClipY/2,FLinearColor::White);
 DrawLine(Canvas->ClipX/2,Canvas->ClipY/2-7,Canvas->ClipX/2,Canvas->ClipY/2+7,FLinearColor::White);
}
AColorLabGameMode::AColorLabGameMode(){DefaultPawnClass=AColorLabCharacter::StaticClass();HUDClass=AColorLabHUD::StaticClass();}
