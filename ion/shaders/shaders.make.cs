using System.Collections.Generic;
using Sharpmake;
using System;
using System.Linq;
using System.IO;

[Generate]
class IonShaders : IonLib
{
	public IonShaders() : base("shaders")
	{
		AddTargets(Globals.IonTargetsDefault);
		SourceFilesExtensions.Add(".glsl");
	}

	protected override void ExcludeOutputFiles()
	{
		base.ExcludeOutputFiles();
		BuildShader.ConfigureShaders(this);
	}

	[Configure]
	public override void Configure(Project.Configuration conf, Target target)
	{
		base.Configure(conf, target);
	}
}