using UnrealBuildTool;
public class ColorLab415EditorTarget : TargetRules {
 public ColorLab415EditorTarget(TargetInfo Target) : base(Target) {
 Type=TargetType.Editor; DefaultBuildSettings=BuildSettingsVersion.V7;
 IncludeOrderVersion=EngineIncludeOrderVersion.Unreal5_8; ExtraModuleNames.Add("ColorLab415");
 }
}
