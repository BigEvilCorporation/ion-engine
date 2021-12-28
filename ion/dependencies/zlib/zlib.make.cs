using Sharpmake;

namespace Dependencies
{
    [Export]
    class LibZLib : IonDependencyLib
    {
        public LibZLib() : base("zlib")
        {
            AddTargets(Globals.IonTargetsDependencyLib);
        }

        [Configure]
        public override void Configure(Project.Configuration conf, Target target)
        {
            base.Configure(conf, target);

            // Defines
            conf.ExportDefines.Add("ION_SUPPORTS_ZLIB");

            // Include paths
            conf.IncludePaths.Add("include");

            // Lib paths
            if (target.Platform == Platform.win32)
                conf.LibraryPaths.Add("lib/win32");
            else if (target.Platform == Platform.win64)
                conf.LibraryPaths.Add("lib/win64");
            else if (target.Platform == Platform.nx)
                conf.LibraryPaths.Add("lib/nx64");

            // Libs
            if (target.Platform == Platform.win32 || target.Platform == Platform.win64)
            {
                if(target.Optimization == Optimization.Debug)
                    conf.LibraryFiles.Add("zlibd.lib");
                else
                    conf.LibraryFiles.Add("zlib.lib");
            }
            else if(target.Platform == Platform.nx)
            {
                conf.LibraryFiles.Add("zlib");
            }
        }
    }
}
