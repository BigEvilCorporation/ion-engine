using System.Collections.Generic;
using Sharpmake;

[Generate]
class IonInput : IonLib
{
    public IonInput() : base("input")
    {
        AddTargets(Globals.IonTargetsDefault);
    }

    [Configure]
    public override void Configure(Project.Configuration conf, Target target)
    {
        base.Configure(conf, target);

        if (target.Platform == Platform.win32 || target.Platform == Platform.win64)
        {
            conf.Defines.Add("ION_INPUT_XINPUT");
            conf.Defines.Add("ION_INPUT_SDL");
            // TODO
            // conf.ExportDefines.Add("ION_INPUT_STEAM");
        }
        else if (target.Platform == Platform.nx)
        {
            conf.Defines.Add("ION_INPUT_SWITCH");
            conf.Defines.Add("ION_INPUT_SWITCH_SHOW_APPLET"); // Show controller settings applet if configuration changes
        }

        var excludedFileSuffixes = new List<string>();
        var excludedFolders = new List<string>();

        if (target.Platform != Platform.win32 && target.Platform != Platform.win64)
        {
            excludedFileSuffixes.Add("xinput");
            excludedFolders.Add("xinput");
        }
        
        // TODO
        excludedFileSuffixes.Add("steam");
        excludedFolders.Add("steam");

        conf.SourceFilesBuildExcludeRegex.Add(@"\.*_(" + string.Join("|", excludedFileSuffixes.ToArray()) + @")\.cpp$");
        conf.SourceFilesBuildExcludeRegex.Add(@"\.*\\(" + string.Join("|", excludedFolders.ToArray()) + @")\\");

        if (target.Platform == Platform.win32 || target.Platform == Platform.win64)
        {
            conf.AddPublicDependency<Dependencies.LibSDL2>(target);
            conf.AddPublicDependency<Dependencies.LibXInput>(target);
        }
    }
}