import unreal as u
tools=u.AssetToolsHelpers.get_asset_tools(); lib=u.MaterialEditingLibrary
folder='/Game/Week3';u.EditorAssetLibrary.make_directory(folder)
def mat(name,dust=False):
 m=u.load_asset(folder+'/'+name) or tools.create_asset(name,folder,u.Material,u.MaterialFactoryNew())
 lib.delete_all_material_expressions(m)
 m.set_editor_property('shading_model',u.MaterialShadingModel.MSM_UNLIT)
 m.set_editor_property('blend_mode',u.BlendMode.BLEND_TRANSLUCENT)
 m.set_editor_property('two_sided',True)
 lib.set_material_usage(m,u.MaterialUsage.MATUSAGE_NIAGARA_SPRITES)
 def n(cls,x,y,**props):
  a=lib.create_material_expression(m,cls,x,y)
  for k,v in props.items():a.set_editor_property(k,v)
  return a
 color=n(u.MaterialExpressionVectorParameter,-450,0,parameter_name='ParticleColor',default_value=u.LinearColor(1,1,1,1))
 lib.connect_material_property(color,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 tex=n(u.MaterialExpressionTextureSample,-450,180,texture=u.load_asset('/Game/Week2/T_Splat0'))
 pc=n(u.MaterialExpressionParticleColor,-450,380)
 mul=n(u.MaterialExpressionMultiply,-150,200)
 lib.connect_material_expressions(tex,'R',mul,'A');lib.connect_material_expressions(pc,'A',mul,'B')
 lib.connect_material_property(mul,'',u.MaterialProperty.MP_OPACITY)
 lib.recompile_material(m);u.EditorAssetLibrary.save_loaded_asset(m)
 return m
paint=mat('M_PaintParticle');dust=mat('M_Dust',True)
source=u.load_asset('/Niagara/DefaultAssets/Templates/Systems/DirectionalBurst')
assert source,'Directional burst template not found'
splat=u.load_asset(folder+'/NS_PaintSplat') or tools.duplicate_asset('NS_PaintSplat',folder,source)
u.ParticleAssetTools.configure_system(splat,paint,None);u.EditorAssetLibrary.save_loaded_asset(splat)
ds=u.load_asset(folder+'/NS_Dust')
if not ds:
 ds=tools.create_asset('NS_Dust',folder,u.NiagaraSystem,u.NiagaraSystemFactoryNew())
 u.ParticleAssetTools.configure_system(ds,dust,u.load_asset('/Niagara/DefaultAssets/Templates/Emitters/HangingParticulates'))
else:u.ParticleAssetTools.configure_system(ds,dust,None)
u.EditorAssetLibrary.save_loaded_asset(ds)
level=u.get_editor_subsystem(u.LevelEditorSubsystem)
if not u.EditorAssetLibrary.does_asset_exist(folder+'/L_ParticleLab'):
 u.EditorAssetLibrary.duplicate_asset('/Game/Week2/L_ColorLab',folder+'/L_ParticleLab')
level.load_level(folder+'/L_ParticleLab')
actors=u.get_editor_subsystem(u.EditorActorSubsystem)
if not any(a.get_class().get_name()=='DustParticleZone' for a in actors.get_all_level_actors()):
 a=actors.spawn_actor_from_class(u.load_class(None,'/Script/ColorLab415.DustParticleZone'),u.Vector(-150,-350,150));a.set_actor_label('Walk through floating dust to recolor')
level.save_current_level()
u.log('WEEK3_ASSETS_COMPLETE')
