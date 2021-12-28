using System.Collections.Generic;
using Sharpmake;

namespace Dependencies
{
    [Generate]
    class LibIMGUI : IonDependencySrc
    {
        public LibIMGUI() : base("imgui")
        {
            AddTargets(Globals.IonTargetsDefault);
        }

        [Configure]
        public override void Configure(Project.Configuration conf, Target target)
        {
            base.Configure(conf, target);

            var excludedFileSuffixes = new List<string>();

            // Defines
            conf.ExportDefines.Add("ION_GUI_SUPPORTS_IMGUI");

            // Include paths
            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]");

            // Source
            SourceFiles = new Strings
            {
                "imgui.cpp",
                "imgui.h",
                "imgui_draw.cpp",
                "imgui_widgets.cpp",
                "imgui_internal.h",
                "imconfig.h",
                "stb_rect_pack.h",
                "stb_textedit.h",
                "stb_truetype.h",
                "imgui_memory_editor/imgui_memory_editor.h",
                "ImGuiColorTextEdit/TextEditor.cpp",
                "ImGuiColorTextEdit/TextEditor.h"
            };

            // Dependencies
            if (target.Platform == Platform.win32 || target.Platform == Platform.win64)
            {
                // Freetype currently only supported on Windows
                conf.AddPublicDependency<Dependencies.LibFreetype>(target);
            }
            else
            {
                excludedFileSuffixes.Add("freetype");
            }

            conf.SourceFilesBuildExcludeRegex.Add(@"\.*_(" + string.Join("|", excludedFileSuffixes.ToArray()) + @")\.cpp$");
        }
    }
}