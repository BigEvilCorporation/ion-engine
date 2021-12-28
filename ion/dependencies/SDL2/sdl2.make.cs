using Sharpmake;

namespace Dependencies
{
    [Export]
    class LibSDL2 : IonDependencyLib
    {
        public LibSDL2() : base("sdl2")
        {
            AddTargets(Globals.IonTargetsDependencyLib);
        }

        [Configure]
        public override void Configure(Project.Configuration conf, Target target)
        {
            base.Configure(conf, target);

            // Defines
            conf.ExportDefines.Add("ION_INPUT_SUPPORTS_SDL2");
            conf.ExportDefines.Add("ION_RENDER_SUPPORTS_SDL2");
            conf.ExportDefines.Add("ION_AUDIO_SUPPORTS_SDL2");

            // Include paths
            conf.IncludePaths.Add("include");
            conf.IncludePaths.Add("include/SDL2");

            // Lib paths
            if (target.Platform == Platform.win32)
                conf.LibraryPaths.Add("lib/win32");
            else if (target.Platform == Platform.win64)
                conf.LibraryPaths.Add("lib/win64");

            // Libs
            if (target.Platform == Platform.win32 || target.Platform == Platform.win64)
            {
                if(target.Optimization == Optimization.Debug)
                    conf.LibraryFiles.Add("SDL2-staticd.lib");
                else
                    conf.LibraryFiles.Add("SDL2-static.lib");

                conf.LibraryFiles.Add("imm32.lib");
                conf.LibraryFiles.Add("winmm.lib");
                conf.LibraryFiles.Add("version.lib");
            }
        }
    }
}