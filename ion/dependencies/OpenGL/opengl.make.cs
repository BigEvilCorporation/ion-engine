using Sharpmake;

namespace Dependencies
{
    [Export]
    class LibOpenGL : IonDependencyLib
    {
        public LibOpenGL() : base("opengl")
        {
            AddTargets(Globals.IonTargetsDependencyLib);
        }

        [Configure]
        public override void Configure(Project.Configuration conf, Target target)
        {
            base.Configure(conf, target);

            // Defines
            conf.ExportDefines.Add("ION_RENDERER_OPENGL_CORE");

            // Include paths
            conf.IncludePaths.Add("include");

            // Libs
            if (target.Platform == Platform.win32 || target.Platform == Platform.win64)
            {
                conf.LibraryFiles.Add("OpenGL32.lib");
                conf.LibraryFiles.Add("glu32.lib");
            }
        }
    }
}

