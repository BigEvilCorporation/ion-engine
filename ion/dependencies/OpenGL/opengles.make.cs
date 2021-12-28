using Sharpmake;

namespace Dependencies
{
    [Export]
    class LibOpenGLES : IonDependencyLib
    {
        public LibOpenGLES() : base("opengles")
        {
            AddTargets(Globals.IonTargetsDependencyLib);
        }

        [Configure]
        public override void Configure(Project.Configuration conf, Target target)
        {
            base.Configure(conf, target);

            // Defines
            conf.ExportDefines.Add("ION_RENDERER_OPENGL_ES");

            // Libs
            if(target.Platform == Platform.nx)
            {
                conf.LibraryFiles.Add("nn_gll");
                conf.LibraryFiles.Add("nn_gfx");
                conf.LibraryFiles.Add("nn_gfxGL");
            }
        }
    }
}
