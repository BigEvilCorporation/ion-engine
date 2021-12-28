using System.Collections.Generic;
using Sharpmake;

[Generate]
class IonServices : IonLib
{
    public IonServices() : base("services")
    {
        AddTargets(Globals.IonTargetsDefault);
    }

    [Configure]
    public override void Configure(Project.Configuration conf, Target target)
    {
        base.Configure(conf, target);

        conf.ExportDefines.Add("ION_SERVICES");

        var excludedFileSuffixes = new List<string>();
        var excludedFolders = new List<string>();

        if (target.Platform != Platform.win32 && target.Platform != Platform.win64)
        {
            // TODO
        }

        // TODO
        excludedFileSuffixes.Add("steam");
        excludedFileSuffixes.Add("galaxy");
        excludedFolders.Add("steam");
        excludedFolders.Add("galaxy");

        if (target.Platform == Platform.nx)
        {
            excludedFileSuffixes.Add("null");
            excludedFolders.Add("null");
        }

        conf.SourceFilesBuildExcludeRegex.Add(@"\.*_(" + string.Join("|", excludedFileSuffixes.ToArray()) + @")\.cpp$");
        conf.SourceFilesBuildExcludeRegex.Add(@"\.*\\(" + string.Join("|", excludedFolders.ToArray()) + @")\\");
    }
}
