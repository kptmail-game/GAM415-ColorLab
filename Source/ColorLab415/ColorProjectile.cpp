#include "ColorProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

AColorProjectile::AColorProjectile()
{
 // Collision drives movement; the visible sphere never causes a second hit.
 Collision=CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
 SetRootComponent(Collision); Collision->InitSphereRadius(12.f);
 Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
 Collision->OnComponentHit.AddDynamic(this,&AColorProjectile::OnHit);
 Mesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
 Mesh->SetupAttachment(Collision); Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
 Mesh->SetStaticMesh(Sphere.Object); Mesh->SetRelativeScale3D(FVector(.24f));
 Movement=CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
 Movement->SetUpdatedComponent(Collision); Movement->InitialSpeed=1200.f;
 Movement->MaxSpeed=1200.f; Movement->ProjectileGravityScale=0.f;
 Movement->bRotationFollowsVelocity=true; InitialLifeSpan=5.f;
}
void AColorProjectile::BeginPlay()
{
 Super::BeginPlay();
 Collision->IgnoreActorWhenMoving(GetOwner(),true);
 // Full saturation avoids nearly gray shots. The hue is selected once per shot.
 ShotColor=FLinearColor::MakeFromHSV8(uint8(FMath::RandRange(0,255)),230,255);
 if(UMaterialInterface* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Week2/M_Projectile.M_Projectile")))
 {
  ProjectileDMI=UMaterialInstanceDynamic::Create(Base,this);
  ProjectileDMI->SetVectorParameterValue(TEXT("Color"),ShotColor);
  Mesh->SetMaterial(0,ProjectileDMI);
 }
 // Parameter updates go through Unreal's rendering abstraction, not direct DX calls.
 UE_LOG(LogTemp,Display,TEXT("SHOT color=%s"),*ShotColor.ToString());
}
void AColorProjectile::OnHit(UPrimitiveComponent*,AActor* OtherActor,UPrimitiveComponent*,FVector,const FHitResult& Hit)
{
 if(bImpacted || OtherActor==GetOwner()) return;
 bImpacted=true;
 UMaterialInterface* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Week2/M_Splat.M_Splat"));
 if(Base)
 {
  // Decal local X projects into the surface, opposite the outward impact normal.
  FRotator Rotation=(-Hit.ImpactNormal).Rotation(); Rotation.Roll=FMath::FRandRange(0.f,360.f);
  UDecalComponent* Decal=UGameplayStatics::SpawnDecalAtLocation(GetWorld(),Base,FVector(16,75,75),Hit.ImpactPoint+Hit.ImpactNormal,Rotation,90.f);
  if(Decal)
  {
   UMaterialInstanceDynamic* DecalDMI=Decal->CreateDynamicMaterialInstance();
   const int32 TextureIndex=FMath::RandRange(0,3);
   const FString TexturePath=FString::Printf(TEXT("/Game/Week2/T_Splat%d.T_Splat%d"),TextureIndex,TextureIndex);
   DecalDMI->SetTextureParameterValue(TEXT("SplatTexture"),LoadObject<UTexture2D>(nullptr,*TexturePath));
   DecalDMI->SetVectorParameterValue(TEXT("Color"),ShotColor);
   Decal->SetFadeScreenSize(0.f);
   // Logging the applied values makes the color invariant easy to verify.
   const FLinearColor Applied=DecalDMI->K2_GetVectorParameterValue(TEXT("Color"));
   ensureAlwaysMsgf(Applied.Equals(ShotColor),TEXT("Projectile/decal colors must match"));
   UE_LOG(LogTemp,Display,TEXT("IMPACT texture=%d projectile=%s decal=%s MATCH=%d"),TextureIndex,*ShotColor.ToString(),*Applied.ToString(),Applied.Equals(ShotColor));
  }
 }
 // Spawn independently of the projectile so destruction cannot remove the burst.
 UNiagaraSystem* Splat=LoadObject<UNiagaraSystem>(nullptr,TEXT("/Game/Week3/NS_PaintSplat.NS_PaintSplat"));
 UMaterialInterface* ParticleBase=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Week3/M_PaintParticle.M_PaintParticle"));
 if(Splat && ParticleBase)
 {
  // Template emits along local Z; orient that axis away from the hit surface.
  const FRotator BurstRotation=FRotationMatrix::MakeFromZ(Hit.ImpactNormal).Rotator();
  UNiagaraComponent* Burst=UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),Splat,Hit.ImpactPoint+Hit.ImpactNormal*3.f,BurstRotation,FVector(.7f),true,false,ENCPoolMethod::None,false);
  if(Burst)
  {
   UMaterialInstanceDynamic* Paint=UMaterialInstanceDynamic::Create(ParticleBase,Burst);
   Paint->SetVectorParameterValue(TEXT("ParticleColor"),ShotColor);
   Burst->SetVariableMaterial(TEXT("User.ParticleMaterial"),Paint);
   // Apply the shared shot color BEFORE activation to avoid a default-color flash.
   Burst->Activate(true);
   const FLinearColor Applied=Paint->K2_GetVectorParameterValue(TEXT("ParticleColor"));
   ensureAlwaysMsgf(Applied.Equals(ShotColor),TEXT("Particle color must match projectile"));
   UE_LOG(LogTemp,Display,TEXT("PAINT_BURST color=%s MATCH=%d active=%d"),*Applied.ToString(),Applied.Equals(ShotColor),Burst->IsActive());
  }
 }
 Destroy();
}
