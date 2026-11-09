using UnrealBuildTool;
public class ColorLab415 : ModuleRules {
 public ColorLab415(ReadOnlyTargetRules Target) : base(Target) {
 PCHUsage=PCHUsageMode.UseExplicitOrSharedPCHs;
 PublicDependencyModuleNames.AddRange(new string[]{"Core","CoreUObject","Engine","InputCore","Niagara"});
 if(Target.bBuildEditor) PrivateDependencyModuleNames.AddRange(new string[]{"UnrealEd","NiagaraEditor"});
 }}
