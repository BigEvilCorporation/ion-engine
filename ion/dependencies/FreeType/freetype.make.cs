using Sharpmake;

namespace Dependencies
{
    [Export]
    class LibFreetype : IonDependencyLib
    {
        public LibFreetype() : base("freetype")
        {
            AddTargets(Globals.IonTargetsDependencyLib);
        }

        [Configure]
        public override void Configure(Project.Configuration conf, Target target)
        {
            base.Configure(conf, target);

            // Defines
            conf.ExportDefines.Add("ION_GUI_SUPPORTS_FREETYPE");
            conf.ExportDefines.Add("ION_RENDER_SUPPORTS_FREETYPE");

            // Include paths
            conf.IncludePaths.Add("include");

            // Lib paths
            if (target.Platform == Platform.win32)
                conf.LibraryPaths.Add("lib/vc120/Win32");
            else if (target.Platform == Platform.win64)
                conf.LibraryPaths.Add("lib/vc120/Win64");

            // Libs
            if (target.Platform == Platform.win32 || target.Platform == Platform.win64)
            {
                if(target.Optimization == Optimization.Debug)
                    conf.LibraryFiles.Add("freetype27STd.lib");
                else
                    conf.LibraryFiles.Add("freetype27ST.lib");
            }
        }
    }
}
