using System.Collections.Generic;
using Sharpmake;

namespace Dependencies
{
    [Export]
    class LibTinyObjLoader : IonDependencyLib
    {
        public LibTinyObjLoader() : base("tinyobjloader")
        {
            AddTargets(Globals.IonTargetsDependencyLib);
        }

        [Configure]
        public override void Configure(Project.Configuration conf, Target target)
        {
            base.Configure(conf, target);

            // Defines
            conf.ExportDefines.Add("ION_GUI_SUPPORTS_TINYOBJLOADER");

            // Include paths
            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]");
        }
    }
}