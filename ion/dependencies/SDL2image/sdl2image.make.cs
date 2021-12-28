using Sharpmake;

namespace Dependencies
{
    [Export]
    class LibSDL2Image : IonDependencyLib
    {
        public LibSDL2Image() : base("sdl2image")
        {
            AddTargets(Globals.IonTargetsDependencyLib);
        }

        [Configure]
        public override void Configure(Project.Configuration conf, Target target)
        {
            base.Configure(conf, target);

            // Defines
            conf.ExportDefines.Add("ION_RENDER_SUPPORTS_SDL2IMAGE");

            // Include paths
            conf.IncludePaths.Add("include");
            conf.IncludePaths.Add("include/SDL2image");

            // Lib paths
            if (target.Platform == Platform.win32)
                conf.LibraryPaths.Add("lib/win32");
            else if (target.Platform == Platform.win64)
                conf.LibraryPaths.Add("lib/win64");

            // Libs
            if (target.Platform == Platform.win32 || target.Platform == Platform.win64)
            {
                if(target.Optimization == Optimization.Debug)
                    conf.LibraryFiles.Add("SDL2_imaged.lib");
                else
                    conf.LibraryFiles.Add("SDL2_image.lib");
            }

            // Dependencies
            conf.AddPublicDependency<Dependencies.LibSDL2>(target);
        }
    }
}
