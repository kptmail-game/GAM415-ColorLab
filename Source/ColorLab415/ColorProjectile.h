#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ColorProjectile.generated.h"
class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UMaterialInstanceDynamic;
// A single shot owns its color; impact copies it instead of randomizing again.
UCLASS()
class COLORLAB415_API AColorProjectile : public AActor
{
 GENERATED_BODY()
public:
 AColorProjectile();
 virtual void BeginPlay() override;
 UPROPERTY(VisibleAnywhere) TObjectPtr<USphereComponent> Collision;
 UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Mesh;
 UPROPERTY(VisibleAnywhere) TObjectPtr<UProjectileMovementComponent> Movement;
 UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> ProjectileDMI;
 UPROPERTY(VisibleAnywhere) FLinearColor ShotColor;
private:
 bool bImpacted=false;
 UFUNCTION() void OnHit(UPrimitiveComponent* HitComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector Impulse,const FHitResult& Hit);
};
