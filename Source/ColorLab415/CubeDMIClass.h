#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CubeDMIClass.generated.h"
class UBoxComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;
// Tutorial actor: each placed cube has an independent translucent material instance.
UCLASS()
class COLORLAB415_API ACubeDMIClass : public AActor
{
 GENERATED_BODY()
public:
 ACubeDMIClass();
 virtual void BeginPlay() override;
 UPROPERTY(VisibleAnywhere) TObjectPtr<UBoxComponent> Box;
 UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Cube;
 UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> DMI;
private:
 UFUNCTION() void OnOverlap(UPrimitiveComponent* Component,AActor* Other,UPrimitiveComponent* OtherComp,int32 BodyIndex,bool bSweep,const FHitResult& Hit);
};
