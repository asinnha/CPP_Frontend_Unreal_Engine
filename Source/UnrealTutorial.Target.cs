// Sinha Productions

using UnrealBuildTool;
using System.Collections.Generic;

public class UnrealTutorialTarget : TargetRules
{
	public UnrealTutorialTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "UnrealTutorial" } );
	}
}
