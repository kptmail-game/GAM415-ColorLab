#include "ProceduralTerrain.h"
#include "KismetProceduralMeshLibrary.h"
#include "Engine/StaticMesh.h"

AProceduralTerrain::AProceduralTerrain()
{
 PrimaryActorTick.bCanEverTick=false;
 Mesh=CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
 SetRootComponent(Mesh);
 Mesh->SetCollisionProfileName(TEXT("BlockAll"));
 Mesh->bUseComplexAsSimpleCollision=true;
 // Synchronous cooking makes the next player trace see the updated surface immediately.
 Mesh->bUseAsyncCooking=false;
}
void AProceduralTerrain::OnConstruction(const FTransform& Transform)
{
 Super::OnConstruction(Transform); Rebuild();
}
void AProceduralTerrain::BeginPlay(){Super::BeginPlay();Rebuild();}
void AProceduralTerrain::Rebuild()
{
 XSize=FMath::Clamp(XSize,1,128);YSize=FMath::Clamp(YSize,1,128);
 CellSize=FMath::Max(10.f,CellSize);NoiseScale=FMath::Max(.001f,NoiseScale);
 Vertices.Reset();Indices.Reset();UVs.Reset();Normals.Reset();Tangents.Reset();Colors.Reset();
 EditCount=0;Mesh->ClearAllMeshSections();
 if(Exercise==EMeshExercise::Plane)
 {
  // Four shared vertices, two triangles, and one UV coordinate per vertex.
  Vertices={FVector(0,0,0),FVector(0,400,0),FVector(400,0,0),FVector(400,400,0)};
  Indices={0,1,2,2,1,3};UVs={FVector2D(0,0),FVector2D(0,1),FVector2D(1,0),FVector2D(1,1)};
 }
 else if(Exercise==EMeshExercise::Cube)
 {
  // Read LOD 0 / section 0 from a CPU-readable static mesh, without displaying a duplicate.
  if(!SourceCube || !SourceCube->bAllowCPUAccess){UE_LOG(LogTemp,Error,TEXT("CUBE_SOURCE_MISSING_CPU_ACCESS"));return;}
  UKismetProceduralMeshLibrary::GetSectionFromStaticMesh(SourceCube,0,0,Vertices,Indices,Normals,UVs,Tangents);
  for(FVector& V:Vertices)V*=3.f;
 }
 else
 {
  Vertices.Reserve((XSize+1)*(YSize+1));UVs.Reserve((XSize+1)*(YSize+1));
  Indices.Reserve(XSize*YSize*6);
  // Shared indexed grid: one vertex per intersection, not six copies per cell.
  for(int32 X=0;X<=XSize;++X)for(int32 Y=0;Y<=YSize;++Y)
  {
   const float Z=FMath::PerlinNoise2D(FVector2D(X,Y)*NoiseScale+NoiseOffset)*Height;
   Vertices.Add(FVector(X*CellSize,Y*CellSize,Z));UVs.Add(FVector2D(X/8.f,Y/8.f));
  }
  for(int32 X=0;X<XSize;++X)for(int32 Y=0;Y<YSize;++Y)
  {
   const int32 A=X*(YSize+1)+Y,B=A+1,C=A+YSize+1,D=C+1;
   Indices.Append({A,B,C,C,B,D});
  }
 }
 RefreshAttributes();
 Mesh->CreateMeshSection_LinearColor(0,Vertices,Indices,Normals,UVs,Colors,Tangents,true,false);
 if(SurfaceMaterial)Mesh->SetMaterial(0,SurfaceMaterial);
 UE_LOG(LogTemp,Display,TEXT("MESH_BUILD kind=%d X=%d Y=%d vertices=%d triangles=%d"),int32(Exercise),XSize,YSize,Vertices.Num(),Indices.Num()/3);
}
void AProceduralTerrain::RefreshAttributes()
{
 // Geometry edits change lighting directions, so recalculate normals and tangents too.
 UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices,Indices,UVs,Normals,Tangents);
 Colors.SetNum(Vertices.Num());
 for(int32 I=0;I<Vertices.Num();++I)
 {
  const float T=FMath::Clamp((Vertices[I].Z+Height)/FMath::Max(1.f,Height*2),0.f,1.f);
  Colors[I]=Exercise==EMeshExercise::Terrain ? FMath::Lerp(FLinearColor(.24,.10,.035),FLinearColor(.18,.58,.32),T) : FLinearColor(.15,.55,.9);
 }
}
bool AProceduralTerrain::AlterAt(const FVector& WorldPoint,float Direction)
{
 if(Exercise!=EMeshExercise::Terrain || Vertices.IsEmpty())return false;
 // Full inverse transform supports translated, rotated, and scaled terrain actors.
 const FVector Local=GetActorTransform().InverseTransformPosition(WorldPoint);
 const float SafeRadius=FMath::Max(1.f,Radius);int32 Changed=0;
 for(FVector& V:Vertices)
 {
  const float Distance=FVector2D(V.X-Local.X,V.Y-Local.Y).Size();
  if(Distance<SafeRadius)
  {
   // Smooth radial falloff; limit elevation to keep repeated edits bounded.
   const float Weight=FMath::Square(1.f-Distance/SafeRadius);
   V.Z=FMath::Clamp(V.Z+Direction*FMath::Max(1.f,Depth)*Weight,-900.f,900.f);++Changed;
  }
 }
 if(!Changed)return false;
 RefreshAttributes();
 Mesh->UpdateMeshSection_LinearColor(0,Vertices,Normals,UVs,Colors,Tangents,false);++EditCount;
 UE_LOG(LogTemp,Display,TEXT("TERRAIN_EDIT changed=%d direction=%.0f edit=%d vertices=%d"),Changed,Direction,EditCount,Vertices.Num());
 return true;
}
