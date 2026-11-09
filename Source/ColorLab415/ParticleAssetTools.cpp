#include "ParticleAssetTools.h"
#include "NiagaraSystem.h"
#include "NiagaraEmitter.h"
#include "NiagaraSpriteRendererProperties.h"
#include "Materials/MaterialInterface.h"
#if WITH_EDITOR
#include "NiagaraEditorUtilities.h"
#include "NiagaraSystemFactoryNew.h"
#endif
void UParticleAssetTools::ConfigureSystem(UNiagaraSystem* System,UMaterialInterface* Material,UNiagaraEmitter* Emitter)
{
#if WITH_EDITOR
 if(!System || !Material)return;
 if(Emitter)
 {
  // Reuse Epic's hanging-particle simulation and customize our own asset copy.
  FNiagaraEditorUtilities::AddEmitterToSystem(*System,*Emitter,Emitter->GetExposedVersion().VersionGuid);
 }
 FNiagaraVariable MaterialVariable(FNiagaraTypeDefinition(UMaterialInterface::StaticClass()),TEXT("User.ParticleMaterial"));
 System->GetExposedParameters().AddParameter(MaterialVariable);
 System->GetExposedParameters().SetUObject(Material,MaterialVariable);
 const bool bPaint=System->GetName().Contains(TEXT("PaintSplat"));
 FNiagaraVariable DropletSize(FNiagaraTypeDefinition::GetVec2Def(),TEXT("User.DropletSize"));
 DropletSize.SetValue(FVector2f(18.f,18.f));
 if(bPaint)System->GetExposedParameters().AddParameter(DropletSize,true);
 for(FNiagaraEmitterHandle& Handle:System->GetEmitterHandles())
 {
  if(FVersionedNiagaraEmitterData* Data=Handle.GetEmitterData())
   for(UNiagaraRendererProperties* Renderer:Data->GetRenderers())
    if(UNiagaraSpriteRendererProperties* Sprite=Cast<UNiagaraSpriteRendererProperties>(Renderer))
    {
     // Each runtime component supplies its own material, preserving color isolation.
     Sprite->Material=Material;
     Sprite->MaterialUserParamBinding.Parameter=MaterialVariable;
     // Visible rounded splat droplets replace the source template's narrow sparks.
     if(bPaint){Sprite->SpriteSizeBinding.Setup(DropletSize,DropletSize);Sprite->Alignment=ENiagaraSpriteAlignment::Unaligned;}
    }
    else
    {
     // The source burst includes decorative trails; keep only paint-colored sprites.
     Renderer->SetIsEnabled(false);
    }
 }
 System->RequestCompile(true);
 System->WaitForCompilationComplete(false,false);
 System->MarkPackageDirty();
#endif
}
