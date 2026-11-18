"""Create the Week 4 material, CPU-readable source cube, and procedural test map."""
import unreal as u
folder='/Game/Week4';u.EditorAssetLibrary.make_directory(folder)
tools=u.AssetToolsHelpers.get_asset_tools();lib=u.MaterialEditingLibrary
rifle=u.load_asset(folder+'/M_Rifle') or tools.create_asset('M_Rifle',folder,u.Material,u.MaterialFactoryNew())
lib.delete_all_material_expressions(rifle)
steel=lib.create_material_expression(rifle,u.MaterialExpressionConstant3Vector,-200,0)
steel.set_editor_property('constant',u.LinearColor(.12,.16,.20,1))
lib.connect_material_property(steel,'',u.MaterialProperty.MP_BASE_COLOR)
lib.connect_material_property(steel,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
lib.recompile_material(rifle);u.EditorAssetLibrary.save_loaded_asset(rifle)
m=u.load_asset(folder+'/M_Terrain') or tools.create_asset('M_Terrain',folder,u.Material,u.MaterialFactoryNew())
lib.delete_all_material_expressions(m)
m.set_editor_property('two_sided',True)
color=lib.create_material_expression(m,u.MaterialExpressionVertexColor,-350,0)
assert lib.connect_material_property(color,'',u.MaterialProperty.MP_BASE_COLOR)
# A subtle emissive fill keeps terrain readable without baked lighting in this demonstration.
fill=lib.create_material_expression(m,u.MaterialExpressionMultiply,-100,100)
fill.set_editor_property('const_b',.7)
assert lib.connect_material_expressions(color,'',fill,'A')
lib.connect_material_property(fill,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
rough=lib.create_material_expression(m,u.MaterialExpressionConstant,-350,150);rough.set_editor_property('r',.85)
lib.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS)
lib.recompile_material(m);u.EditorAssetLibrary.save_loaded_asset(m)
cube=u.load_asset(folder+'/SM_CPUCube') or tools.duplicate_asset('SM_CPUCube',folder,u.load_asset('/Engine/BasicShapes/Cube'))
cube.set_editor_property('allow_cpu_access',True);u.EditorAssetLibrary.save_loaded_asset(cube)
level=u.get_editor_subsystem(u.LevelEditorSubsystem)
if u.EditorAssetLibrary.does_asset_exist(folder+'/L_TerrainLab'):
 level.load_level(folder+'/L_TerrainLab')
 u.log('WEEK4_ASSETS_UPDATED')
else:
 level.new_level(folder+'/L_TerrainLab')
actors=u.get_editor_subsystem(u.EditorActorSubsystem)
# Rebuild only this generated lab's actors on repeat runs.
for existing in actors.get_all_level_actors():
 if existing.get_class().get_name() in ['ProceduralTerrain','DirectionalLight','SkyLight','SkyAtmosphere','PlayerStart'] or existing.get_actor_label()=='Rifle Firing Platform':actors.destroy_actor(existing)
cls=u.load_class(None,'/Script/ColorLab415.ProceduralTerrain')
def mesh(label,kind,pos):
 a=actors.spawn_actor_from_class(cls,u.Vector(*pos));a.set_actor_label(label)
 if kind==u.MeshExercise.CUBE:a.set_editor_property('source_cube',cube)
 a.set_editor_property('exercise',kind);a.set_editor_property('surface_material',m)
 a.rebuild();return a
t=mesh('Editable Perlin Terrain - XSize 64 YSize 48',u.MeshExercise.TERRAIN,(0,0,0))
mesh('Tutorial Plane - four vertices two triangles',u.MeshExercise.PLANE,(-650,200,80))
mesh('Tutorial Cube - copied static mesh data',u.MeshExercise.CUBE,(-450,1050,180))
sun=actors.spawn_actor_from_class(u.DirectionalLight,u.Vector(0,0,1500),u.Rotator(-45,-35,0))
sun.light_component.set_editor_property('intensity',4)
sky=actors.spawn_actor_from_class(u.SkyLight,u.Vector(0,0,1200));sky.light_component.set_editor_property('intensity',1)
actors.spawn_actor_from_class(u.SkyAtmosphere,u.Vector(0,0,0))
platform=actors.spawn_actor_from_class(u.StaticMeshActor,u.Vector(-450,1400,300))
platform.set_actor_label('Rifle Firing Platform')
platform.static_mesh_component.set_static_mesh(u.load_asset('/Engine/BasicShapes/Cube'))
platform.set_actor_scale3d(u.Vector(10,18,1))
platform.static_mesh_component.set_material(0,rifle)
start=actors.spawn_actor_from_class(u.PlayerStart,u.Vector(-350,1400,450),u.Rotator(-18,0,0))
u.get_editor_subsystem(u.UnrealEditorSubsystem).set_level_viewport_camera_info(u.Vector(-1500,-1800,2200),u.Rotator(-30,40,0))
world=u.get_editor_subsystem(u.UnrealEditorSubsystem).get_editor_world()
world.get_world_settings().set_editor_property('default_game_mode',u.load_class(None,'/Script/ColorLab415.TerrainLabMode'))
level.save_current_level();u.log('WEEK4_ASSETS_COMPLETE')


