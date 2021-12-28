using Sharpmake;

namespace Dependencies
{
    [Export]
    class LibWxWidgets : IonDependencyLib
    {
        public LibWxWidgets() : base("wxwidgets")
        {
            AddTargets(Globals.IonTargetsDependencyLib);
        }

        [Configure]
        public override void Configure(Project.Configuration conf, Target target)
        {
            base.Configure(conf, target);

            // Defines
            conf.ExportDefines.Add("_CRT_SECURE_NO_WARNINGS");
            //conf.ExportDefines.Add("WXUSINGDLL");
            //conf.ExportDefines.Add("wxMSVC_VERSION_AUTO");
            //conf.ExportDefines.Add("wxMSVC_VERSION_ABI_COMPAT");

            if (target.Platform == Platform.win32 || target.Platform == Platform.win64)
            {
                conf.ExportDefines.Add("__WXMSW__");
            }

            // Include paths
            conf.IncludePaths.Add("include");

            if (target.Platform == Platform.win32 || target.Platform == Platform.win64)
            {
                conf.IncludePaths.Add("include/msvc");
            }

            // Libs
            if (target.Platform == Platform.win32 || target.Platform == Platform.win64)
            {
                conf.LibraryPaths.Add("lib/vc_x64_lib");

                //conf.LibraryFiles.Add("Comctl32.lib");
                //conf.LibraryFiles.Add("Rpcrt4.lib");

                if (target.Optimization == Optimization.Debug)
                {
                    conf.LibraryFiles.Add("wxbase31ud.lib");
                    conf.LibraryFiles.Add("wxbase31ud_net.lib");
                    conf.LibraryFiles.Add("wxbase31ud_xml.lib");
                    conf.LibraryFiles.Add("wxmsw31ud_adv.lib");
                    conf.LibraryFiles.Add("wxmsw31ud_aui.lib");
                    conf.LibraryFiles.Add("wxmsw31ud_core.lib");
                    conf.LibraryFiles.Add("wxmsw31ud_gl.lib");
                    conf.LibraryFiles.Add("wxmsw31ud_html.lib");
                    conf.LibraryFiles.Add("wxmsw31ud_media.lib");
                    conf.LibraryFiles.Add("wxmsw31ud_propgrid.lib");
                    conf.LibraryFiles.Add("wxmsw31ud_qa.lib");
                    conf.LibraryFiles.Add("wxmsw31ud_ribbon.lib");
                    conf.LibraryFiles.Add("wxmsw31ud_richtext.lib");
                    conf.LibraryFiles.Add("wxmsw31ud_stc.lib");
                    conf.LibraryFiles.Add("wxmsw31ud_webview.lib");
                    conf.LibraryFiles.Add("wxmsw31ud_xrc.lib");
                    conf.LibraryFiles.Add("wxexpatd.lib");
                    conf.LibraryFiles.Add("wxjpegd.lib");
                    conf.LibraryFiles.Add("wxpngd.lib");
                    conf.LibraryFiles.Add("wxregexud.lib");
                    conf.LibraryFiles.Add("wxscintillad.lib");
                    conf.LibraryFiles.Add("wxtiffd.lib");
                    conf.LibraryFiles.Add("wxzlibd.lib");
                }
                else
                {
                    conf.LibraryFiles.Add("wxbase31u.lib");
                    conf.LibraryFiles.Add("wxbase31u_net.lib");
                    conf.LibraryFiles.Add("wxbase31u_xml.lib");
                    conf.LibraryFiles.Add("wxmsw31u_adv.lib");
                    conf.LibraryFiles.Add("wxmsw31u_aui.lib");
                    conf.LibraryFiles.Add("wxmsw31u_core.lib");
                    conf.LibraryFiles.Add("wxmsw31u_gl.lib");
                    conf.LibraryFiles.Add("wxmsw31u_html.lib");
                    conf.LibraryFiles.Add("wxmsw31u_media.lib");
                    conf.LibraryFiles.Add("wxmsw31u_propgrid.lib");
                    conf.LibraryFiles.Add("wxmsw31u_qa.lib");
                    conf.LibraryFiles.Add("wxmsw31u_ribbon.lib");
                    conf.LibraryFiles.Add("wxmsw31u_richtext.lib");
                    conf.LibraryFiles.Add("wxmsw31u_stc.lib");
                    conf.LibraryFiles.Add("wxmsw31u_webview.lib");
                    conf.LibraryFiles.Add("wxmsw31u_xrc.lib");
                    conf.LibraryFiles.Add("wxexpat.lib");
                    conf.LibraryFiles.Add("wxjpeg.lib");
                    conf.LibraryFiles.Add("wxpng.lib");
                    conf.LibraryFiles.Add("wxregexu.lib");
                    conf.LibraryFiles.Add("wxscintilla.lib");
                    conf.LibraryFiles.Add("wxtiff.lib");
                    conf.LibraryFiles.Add("wxzlib.lib");
                }
            }
        }
    }
}