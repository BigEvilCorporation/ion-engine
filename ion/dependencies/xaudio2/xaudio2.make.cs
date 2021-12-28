using Sharpmake;

namespace Dependencies
{
    [Export]
    class LibXAudio2 : IonDependencyLib
    {
        public LibXAudio2() : base("xaudio2")
        {
            AddTargets(Globals.IonTargetsDependencyLib);
        }

        [Configure]
        public override void Configure(Project.Configuration conf, Target target)
        {
            base.Configure(conf, target);

            // Defines
            conf.ExportDefines.Add("ION_AUDIO_SUPPORTS_XAUDIO2");

            // Include paths
            conf.IncludePaths.Add("include");

            // Lib paths
            if (target.Platform == Platform.win32)
                conf.LibraryPaths.Add("release/lib/x86");
            else if (target.Platform == Platform.win64)
                conf.LibraryPaths.Add(@"release/lib/x64");

            // Libs
            if (target.Platform == Platform.win32 || target.Platform == Platform.win64)
            {
                conf.LibraryFiles.Add("xaudio2_9redist.lib");
                conf.LibraryFiles.Add("xapobaseredist.lib");
            }

            // DLLs
            if (target.Platform == Platform.win32)
                conf.TargetCopyFiles.Add("release/bin/x86/xaudio2_9redist.dll");
            else if (target.Platform == Platform.win64)
                conf.TargetCopyFiles.Add("release/bin/x64/xaudio2_9redist.dll");
        }
    }
}
