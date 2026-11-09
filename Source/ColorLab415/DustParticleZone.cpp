#include "DustParticleZone.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/Pawn.h"
ADustParticleZone::ADustParticleZone()
{
 Trigger=CreateDefaultSubobject<UBoxComponent>(TEXT("DustTrigger"));SetRootComponent(Trigger);
 Trigger->SetBoxExtent(FVector(170,170,140));Trigger->SetCollisionProfileName(TEXT("Trigger"));
 Trigger->OnComponentBeginOverlap.AddDynamic(this,&ADustParticleZone::OnOverlap);
 Dust=CreateDefaultSubobject<UNiagaraComponent>(TEXT("FloatingDust"));Dust->SetupAttachment(Trigger);
 Dust->SetAutoActivate(false);
}
void ADustParticleZone::BeginPlay()
{
 Super::BeginPlay();
 UNiagaraSystem* System=LoadObject<UNiagaraSystem>(nullptr,TEXT("/Game/Week3/NS_Dust.NS_Dust"));
 UMaterialInterface* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Week3/M_Dust.M_Dust"));
 if(!System || !Base){UE_LOG(LogTemp,Error,TEXT("Dust assets missing"));return;}
 DustMaterial=UMaterialInstanceDynamic::Create(Base,this);
 DustMaterial->SetVectorParameterValue(TEXT("ParticleColor"),FLinearColor(.1f,.7f,1.f));
 Dust->SetAsset(System);Dust->SetVariableMaterial(TEXT("User.ParticleMaterial"),DustMaterial);
 Dust->Activate(true);
}
void ADustParticleZone::OnOverlap(UPrimitiveComponent*,AActor* Other,UPrimitiveComponent*,int32,bool,const FHitResult&)
{
 const APawn* Pawn=Cast<APawn>(Other);
 if(!Pawn || !Pawn->IsPlayerControlled() || !DustMaterial)return;
 // Updating this instance recolors live particles as well as newly spawned ones.
 const FLinearColor Color=FLinearColor::MakeFromHSV8(uint8(FMath::RandRange(0,255)),230,255);
 DustMaterial->SetVectorParameterValue(TEXT("ParticleColor"),Color);
 UE_LOG(LogTemp,Display,TEXT("DUST_OVERLAP color=%s active=%d"),*Color.ToString(),Dust->IsActive());
}
