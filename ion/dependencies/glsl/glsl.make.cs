using Sharpmake;

namespace Dependencies
{
    [Export]
    class LibGLSL : IonDependencyLib
    {
        public LibGLSL() : base("glsl")
        {
            AddTargets(Globals.IonTargetsDependencyLib);
        }

        [Configure]
        public override void Configure(Project.Configuration conf, Target target)
        {
            base.Configure(conf, target);

            // Defines
            conf.ExportDefines.Add("ION_RENDERER_SHADER");
            conf.ExportDefines.Add("ION_SHADER_LANG_GLSL");
        }
    }
}