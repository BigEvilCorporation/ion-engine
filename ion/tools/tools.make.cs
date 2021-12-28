using System.IO;
using System.Collections.Generic;
using Sharpmake;

[module: Sharpmake.Include("../ion.make.cs")]

public static class GlobalsTools
{
    public static Target[] IonTargetsDefault
    {
        get
        {
            return new Target[] { new Target(
                        Platform.win64,
                        DevEnv.vs2019,
                        Optimization.Debug | Optimization.Release,
                        OutputType.Lib ) };
        }
    }
}

[Generate]
class IonBuildResource : IonExe
{
    public IonBuildResource() : base("BuildResource", "BuildResource")
    {
        AddTargets(GlobalsTools.IonTargetsDefault);
    }

    [Configure]
    public override void Configure(Project.Configuration conf, Target target)
    {
        conf.TargetPath = Path.Combine(Globals.RootDir, "ion", "tools", "buildresource");

        base.Configure(conf, target);

        // ion::engine
        conf.AddPublicDependency<IonCore>(target);
        conf.AddPublicDependency<IonRenderer>(target);
        conf.AddPublicDependency<IonResource>(target);

        // Dependencies
        conf.AddPublicDependency<Dependencies.LibPNG>(target);
    }
}
