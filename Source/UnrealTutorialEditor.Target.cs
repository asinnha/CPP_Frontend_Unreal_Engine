// Sinha Productions

using UnrealBuildTool;
using System.Collections.Generic;

public class UnrealTutorialEditorTarget : TargetRules
{
	public UnrealTutorialEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "UnrealTutorial" } );
	}
}
