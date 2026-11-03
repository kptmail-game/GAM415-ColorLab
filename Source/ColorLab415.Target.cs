using UnrealBuildTool;
public class ColorLab415Target : TargetRules {
 public ColorLab415Target(TargetInfo Target) : base(Target) {
 Type=TargetType.Game; DefaultBuildSettings=BuildSettingsVersion.V7;
 IncludeOrderVersion=EngineIncludeOrderVersion.Unreal5_8; ExtraModuleNames.Add("ColorLab415");
 }
}
