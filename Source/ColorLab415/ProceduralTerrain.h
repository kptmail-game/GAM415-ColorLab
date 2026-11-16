#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralTerrain.generated.h"

// One actor demonstrates the two tutorial meshes and the editable terrain.
UENUM(BlueprintType)
enum class EMeshExercise : uint8 { Terrain, Plane, Cube };
UCLASS()
class COLORLAB415_API AProceduralTerrain : public AActor
{
 GENERATED_BODY()
public:
 AProceduralTerrain();
 virtual void OnConstruction(const FTransform& Transform) override;
 virtual void BeginPlay() override;
 UPROPERTY(VisibleAnywhere) TObjectPtr<UProceduralMeshComponent> Mesh;
 UPROPERTY(EditAnywhere,Category="Generation") EMeshExercise Exercise=EMeshExercise::Terrain;
 // Cell counts are bounded in the editor AND code to prevent excessive allocations.
 UPROPERTY(EditAnywhere,Category="Generation",meta=(ClampMin="1",ClampMax="128")) int32 XSize=64;
 UPROPERTY(EditAnywhere,Category="Generation",meta=(ClampMin="1",ClampMax="128")) int32 YSize=48;
 UPROPERTY(EditAnywhere,Category="Generation",meta=(ClampMin="10")) float CellSize=60;
 UPROPERTY(EditAnywhere,Category="Generation",meta=(ClampMin="0")) float Height=240;
 UPROPERTY(EditAnywhere,Category="Generation",meta=(ClampMin="0.001")) float NoiseScale=.075f;
 UPROPERTY(EditAnywhere,Category="Generation") FVector2D NoiseOffset=FVector2D(12.4,8.7);
 UPROPERTY(EditAnywhere,Category="Generation") TObjectPtr<UMaterialInterface> SurfaceMaterial;
 UPROPERTY(EditAnywhere,Category="Generation") TObjectPtr<UStaticMesh> SourceCube;
 UPROPERTY(EditAnywhere,Category="Brush",meta=(ClampMin="1")) float Radius=270;
 UPROPERTY(EditAnywhere,Category="Brush",meta=(ClampMin="1")) float Depth=65;
 UFUNCTION(BlueprintCallable,CallInEditor,Category="Generation") void Rebuild();
 bool AlterAt(const FVector& WorldPoint,float Direction);
 int32 VertexCount() const {return Vertices.Num();}
 int32 TriangleCount() const {return Indices.Num()/3;}
 int32 EditCount=0;
private:
 // Keep topology and UVs between edits; UpdateMeshSection only changes vertex attributes.
 TArray<FVector> Vertices,Normals;
 TArray<int32> Indices;
 TArray<FVector2D> UVs;
 TArray<FLinearColor> Colors;
 TArray<FProcMeshTangent> Tangents;
 void RefreshAttributes();
};
