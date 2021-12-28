using System.Collections.Generic;
using Sharpmake;

namespace Dependencies
{
    [Generate]
    class LibSLZ : IonDependencySrc
    {
        public LibSLZ() : base("slz")
        {
            AddTargets(Globals.IonTargetsDefault);
        }

        [Configure]
        public override void Configure(Project.Configuration conf, Target target)
        {
            base.Configure(conf, target);

            // Defines
            conf.ExportDefines.Add("ION_SUPPORTS_SLZ");

            // Include paths
            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]");

            // Source
            SourceFiles = new Strings
            {
                "tool/compress.h",
                "tool/compress.c",
                "tool/decompress.h",
                "tool/decompress.c",
            };
        }
    }
}
