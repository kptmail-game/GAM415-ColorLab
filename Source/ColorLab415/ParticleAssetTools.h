#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ParticleAssetTools.generated.h"
class UNiagaraSystem;
class UNiagaraEmitter;
class UMaterialInterface;
// Editor-only asset configuration is exposed to the reproducible Python build script.
UCLASS()
class COLORLAB415_API UParticleAssetTools : public UBlueprintFunctionLibrary
{
 GENERATED_BODY()
public:
 UFUNCTION(BlueprintCallable) static void ConfigureSystem(UNiagaraSystem* System, UMaterialInterface* Material, UNiagaraEmitter* Emitter);
};
