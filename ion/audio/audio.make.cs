using System.Collections.Generic;
using Sharpmake;

[Generate]
class IonAudio : IonLib
{
    public IonAudio() : base("audio")
    {
        AddTargets(Globals.IonTargetsDefault);
    }

    [Configure]
    public override void Configure(Project.Configuration conf, Target target)
    {
        base.Configure(conf, target);

        var excludedFileSuffixes = new List<string>();
        var excludedFolders = new List<string>();

        //if (target.Platform != Platform.Dreamcast)
        {
            excludedFileSuffixes.Add("sdl");
            excludedFolders.Add("sdl");
        }

        if (target.Platform != Platform.linux && target.Platform != Platform.mac)
        {
            excludedFileSuffixes.Add("sdl2");
            excludedFolders.Add("sdl2");
        }

        if (target.Platform != Platform.win32 && target.Platform != Platform.win64)
        {
            excludedFileSuffixes.Add("xaudio");
            excludedFolders.Add("xaudio");
        }

        // TODO
        excludedFileSuffixes.Add("steam");
        excludedFolders.Add("steam");

        conf.SourceFilesBuildExcludeRegex.Add(@"\.*_(" + string.Join("|", excludedFileSuffixes.ToArray()) + @")\.cpp$");
        conf.SourceFilesBuildExcludeRegex.Add(@"\.*\\(" + string.Join("|", excludedFolders.ToArray()) + @")\\");

        if (target.Platform == Platform.win32 || target.Platform == Platform.win64)
        {
            conf.AddPublicDependency<Dependencies.LibXAudio2>(target);
        }
    }
}
