using System.IO;
using System.Collections.Generic;
using Sharpmake;

[module: Sharpmake.Include("audio/audio.make.cs")]
[module: Sharpmake.Include("beehive/beehive.make.cs")]
[module: Sharpmake.Include("core/core.make.cs")]
[module: Sharpmake.Include("engine/engine.make.cs")]
[module: Sharpmake.Include("gamekit/gamekit.make.cs")]
[module: Sharpmake.Include("gui/gui.make.cs")]
[module: Sharpmake.Include("input/input.make.cs")]
[module: Sharpmake.Include("maths/maths.make.cs")]
[module: Sharpmake.Include("network/network.make.cs")]
[module: Sharpmake.Include("renderer/renderer.make.cs")]
[module: Sharpmake.Include("resource/resource.make.cs")]
[module: Sharpmake.Include("services/services.make.cs")]
[module: Sharpmake.Include("shaders/shaders.make.cs")]

[module: Sharpmake.Include("dependencies/dependencies.make.cs")]
[module: Sharpmake.Include("assets.make.cs")]
[module: Sharpmake.Include("tools/tools.make.cs")]

public static class Globals
{
    public static string RootDir = Directory.GetCurrentDirectory();
    public static string BuildDir = Path.Combine(RootDir, ".build");
    public static string IonDir = Path.Combine(RootDir, "ion");
    public static string ResourceBuilder => Path.Combine(Globals.RootDir, "ion", "tools", "buildresource", "buildresource.exe");

    public static Target[] IonTargetsDefault
    {
        get
        {
            return new Target[] { new Target( Platform.win32 | Platform.win64,
                        DevEnv.vs2019,
                        Optimization.Debug | Optimization.Release | Optimization.Retail,
                        OutputType.Lib ) };
        }
    }

    public static Target[] IonTargetsDependencyLib
    {
        get
        {
            return new Target[] { new Target( Platform.win32 | Platform.win64,
                        DevEnv.vs2019,
                        Optimization.Debug | Optimization.Release | Optimization.Retail ) };
        }
    }
}

abstract class IonBase : Project
{
    public IonBase(string name, OutputType outputType)
    {
        Name = name;

        // For Windows, set highest available platform SDK
        KitsRootPaths.SetKitsRoot10ToHighestInstalledVersion();
    }

    public virtual void Configure(Project.Configuration conf, Target target)
    {
        if(conf.Output == Configuration.OutputType.Exe)
            conf.Name = @"[target.Optimization]";
        else
            conf.Name = @"[target.Optimization]_[target.OutputType]";

        conf.ProjectPath = Path.Combine(Globals.BuildDir, @"projects/[project.Name]");

        // Global defines
        conf.Defines.Add("ION_ENGINE");

        if (target.Optimization == Optimization.Debug)
            conf.Defines.Add("ION_BUILD_DEBUG");
        else if(target.Optimization == Optimization.Release)
            conf.Defines.Add("ION_BUILD_RELEASE");
        else if(target.Optimization == Optimization.Retail)
            conf.Defines.Add("ION_BUILD_MASTER");

        if (target.Platform == Platform.win32)
        {
            conf.Defines.Add("ION_PLATFORM_WINDOWS");
            conf.Defines.Add("ION_PLATFORM_DESKTOP");
            conf.Defines.Add("ION_PLATFORM_32BIT");
            conf.Defines.Add("ION_ENDIAN_LITTLE");
        }
        else if (target.Platform == Platform.win64)
        {
            conf.Defines.Add("ION_PLATFORM_WINDOWS");
            conf.Defines.Add("ION_PLATFORM_DESKTOP");
            conf.Defines.Add("ION_PLATFORM_64BIT");
            conf.Defines.Add("ION_ENDIAN_LITTLE");
        }
        else if (target.Platform == Platform.nx)
        {
			conf.Defines.Add("ION_PLATFORM_CONSOLE");
			conf.Defines.Add("ION_PLATFORM_SWITCH");
			conf.Defines.Add("ION_PLATFORM_64BIT");
			conf.Defines.Add("ION_ENDIAN_LITTLE");
        }

        // Global include dirs
        conf.IncludePaths.Add(Globals.RootDir);
        conf.IncludePaths.Add(Globals.IonDir);

        // Exclude per-platform directories if non-matching platform
        var excludedFileSuffixes = new List<string>();
        var excludedFolders = new List<string>();

        if (target.Platform != Platform.android)
        {
            excludedFileSuffixes.Add("android");
            excludedFolders.Add("android");
        }

        //if (target.Platform != Platform.Dreamcast)
        {
            excludedFileSuffixes.Add("dreamcast");
            excludedFolders.Add("dreamcast");
        }

        if (target.Platform != Platform.linux)
        {
            excludedFileSuffixes.Add("linux");
            excludedFolders.Add("linux");
        }

        if (target.Platform != Platform.mac)
        {
            excludedFileSuffixes.Add("macosx");
            excludedFolders.Add("macosx");
        }

        //if (target.Platform != Platform.RaspberryPi)
        {
            excludedFileSuffixes.Add("raspberrypi");
            excludedFolders.Add("raspberrypi");
        }

        if (target.Platform != Platform.nx)
        {
            excludedFileSuffixes.Add("nx");
            excludedFileSuffixes.Add("switch");
            excludedFolders.Add("nx");
            excludedFolders.Add("switch");
        }

        if (target.Platform != Platform.win32 && target.Platform != Platform.win64)
        {
            excludedFileSuffixes.Add("windows");
            excludedFileSuffixes.Add("win32");
            excludedFolders.Add("windows");
            excludedFolders.Add("win32");
        }

        conf.SourceFilesBuildExcludeRegex.Add(@"\.*_(" + string.Join("|", excludedFileSuffixes.ToArray()) + @")\.cpp$");
        conf.SourceFilesBuildExcludeRegex.Add(@"\.*\\(" + string.Join("|", excludedFolders.ToArray()) + @")\\");

        // Enable exceptions
        conf.Options.Add(Sharpmake.Options.Vc.Compiler.Exceptions.Enable);

        // Enable RTTI
        conf.Options.Add(Sharpmake.Options.Vc.Compiler.RTTI.Enable);

        // Enable unicode
        conf.Options.Add(Sharpmake.Options.Vc.General.CharacterSet.Unicode);

        // Disable unneccessary warnings
        if (target.Platform == Platform.win32 || target.Platform == Platform.win64)
        {
            conf.Options.Add(new Sharpmake.Options.Vc.Compiler.DisableSpecificWarnings("4100"));
        }
    }
}

abstract class IonLib : IonBase
{
    public IonLib(string name) : base(name, OutputType.Lib)
    {

    }

    public override void Configure(Project.Configuration conf, Target target)
    {
        base.Configure(conf, target);
        conf.SolutionFolder = "ion";
        conf.IncludePaths.Add(@"[project.SharpmakeCsPath]");
        conf.Defines.Add("ION_LIB");
        conf.Output = Configuration.OutputType.Lib;
    }
}

abstract class IonDependencySrc : IonBase
{
    public IonDependencySrc(string name) : base(name, OutputType.Lib)
    {

    }

    public override void Configure(Project.Configuration conf, Target target)
    {
        base.Configure(conf, target);
        conf.SolutionFolder = "dependencies";
        conf.IncludePaths.Add(@"[project.SharpmakeCsPath]");
        conf.Output = Configuration.OutputType.Lib;
    }
}

abstract class IonDependencyLib : Project
{
    public IonDependencyLib(string name)
    {
        Name = name;

        // For Windows, set highest available platform SDK
        KitsRootPaths.SetKitsRoot10ToHighestInstalledVersion();
    }

    public virtual void Configure(Configuration conf, Target target)
    {
        conf.Name = @"[target.Optimization]_[target.OutputType]";
        conf.SolutionFolder = "dependencies";
    }
}

abstract class IonExe : IonBase
{
    public IonExe(string name, string dir) : base(name, OutputType.Lib)
    {
        SourceRootPath = dir;
    }

    public override void Configure(Project.Configuration conf, Target target)
    {
        base.Configure(conf, target);
        conf.IncludePaths.Add(Path.Combine(@"[project.SharpmakeCsPath]", SourceRootPath));
        conf.Output = Configuration.OutputType.Exe;
    }
}

class IonSolution : Solution
{
    public IonSolution(string name)
    {
        Name = name;
    }

    public virtual void Configure(Solution.Configuration conf, Target target)
    {
        conf.Name = @"[target.Optimization]_[target.OutputType]";
        conf.SolutionPath = Globals.BuildDir;
    }
}
