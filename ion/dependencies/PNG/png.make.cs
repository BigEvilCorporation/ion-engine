using System.Collections.Generic;
using Sharpmake;

namespace Dependencies
{
    [Generate]
    class LibPNG : IonDependencySrc
    {
        public LibPNG() : base("png")
        {
            AddTargets(Globals.IonTargetsDefault);
        }

        [Configure]
        public override void Configure(Project.Configuration conf, Target target)
        {
            base.Configure(conf, target);

            // Defines
            conf.ExportDefines.Add("ION_RENDER_SUPPORTS_PNG");

            // Include paths
            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]");

            // Source
            SourceFiles = new Strings
            {
                "png.h",
                "pngconf.h",
                "pngdebug.h",
                "pnginfo.h",
                "pnglibconf.h",
                "pngpriv.h",
                "pngstruct.h",
                "png.c",
                "pngerror.c",
                "pngget.c",
                "pngmem.c",
                "pngpread.c",
                "pngread.c",
                "pngrio.c",
                "pngrtran.c",
                "pngrutil.c",
                "pngset.c",
                "pngtest.c",
                "pngtrans.c",
                "pngwio.c",
                "pngwrite.c",
                "pngwtran.c",
                "pngwutil.c"
            };

            // Exclude per-platform directories if non-matching platform
            var excludedFileSuffixes = new List<string>();
            var excludedFolders = new List<string>();

            if (target.Platform != Platform.win32 && target.Platform != Platform.win64)
            {
                excludedFileSuffixes.Add("intel");
                excludedFolders.Add("intel");
            }

            if (target.Platform != Platform.nx)
            {
                excludedFileSuffixes.Add("arm");
                excludedFileSuffixes.Add("arm-neon");
                excludedFolders.Add("arm");
                excludedFolders.Add("arm-neon");
            }

            excludedFileSuffixes.Add("ndk");

            conf.SourceFilesBuildExcludeRegex.Add(@"\.*_(" + string.Join("|", excludedFileSuffixes.ToArray()) + @")\.cpp$");
            conf.SourceFilesBuildExcludeRegex.Add(@"\.*\\(" + string.Join("|", excludedFolders.ToArray()) + @")\\");

            // Dependencies
            conf.AddPublicDependency<Dependencies.LibZLib>(target);
        }
    }
}
