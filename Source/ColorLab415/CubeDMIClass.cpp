#include "CubeDMIClass.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/Pawn.h"
#include "UObject/ConstructorHelpers.h"
ACubeDMIClass::ACubeDMIClass()
{
 Box=CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox")); SetRootComponent(Box);
 Box->SetBoxExtent(FVector(120)); Box->SetCollisionProfileName(TEXT("Trigger"));
 Box->OnComponentBeginOverlap.AddDynamic(this,&ACubeDMIClass::OnOverlap);
 Cube=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube")); Cube->SetupAttachment(Box);
 Cube->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 static ConstructorHelpers::FObjectFinder<UStaticMesh> Shape(TEXT("/Engine/BasicShapes/Cube.Cube"));
 Cube->SetStaticMesh(Shape.Object); Cube->SetRelativeScale3D(FVector(1.6f));
}
void ACubeDMIClass::BeginPlay()
{
 Super::BeginPlay();
 if(UMaterialInterface* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Week2/M_Cube.M_Cube")))
 {
  DMI=UMaterialInstanceDynamic::Create(Base,this); Cube->SetMaterial(0,DMI);
  DMI->SetVectorParameterValue(TEXT("Color"),FLinearColor(.1f,.7f,1.f));
  DMI->SetScalarParameterValue(TEXT("Opacity"),.6f);
 }
}
void ACubeDMIClass::OnOverlap(UPrimitiveComponent*,AActor* Other,UPrimitiveComponent*,int32,bool,const FHitResult&)
{
 // Only the player changes the cube; projectiles and scenery are ignored.
 APawn* Pawn=Cast<APawn>(Other);
 if(!DMI || !Pawn || !Pawn->IsPlayerControlled()) return;
 const FLinearColor Color(FMath::FRand(),FMath::FRand(),FMath::FRand(),1.f);
 const float Opacity=FMath::FRandRange(.15f,.9f);
 DMI->SetVectorParameterValue(TEXT("Color"),Color);
 DMI->SetScalarParameterValue(TEXT("Opacity"),Opacity);
 UE_LOG(LogTemp,Display,TEXT("CUBE OVERLAP color=%s opacity=%.3f"),*Color.ToString(),Opacity);
}
