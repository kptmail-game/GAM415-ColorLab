#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DustParticleZone.generated.h"
class UBoxComponent;
class UNiagaraComponent;
class UMaterialInstanceDynamic;
// The tutorial's slow floating dust changes color when the player enters its volume.
UCLASS()
class COLORLAB415_API ADustParticleZone : public AActor
{
 GENERATED_BODY()
public:
 ADustParticleZone();
 virtual void BeginPlay() override;
 UPROPERTY(VisibleAnywhere) TObjectPtr<UBoxComponent> Trigger;
 UPROPERTY(VisibleAnywhere) TObjectPtr<UNiagaraComponent> Dust;
 UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> DustMaterial;
private:
 UFUNCTION() void OnOverlap(UPrimitiveComponent* OverlappedComponent,AActor* Other,UPrimitiveComponent* OtherComponent,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
};
