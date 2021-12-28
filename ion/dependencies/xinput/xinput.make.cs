using Sharpmake;

namespace Dependencies
{
    [Export]
    class LibXInput : IonDependencyLib
    {
        public LibXInput() : base("xinput")
        {
            AddTargets(Globals.IonTargetsDependencyLib);
        }

        [Configure]
        public override void Configure(Project.Configuration conf, Target target)
        {
            base.Configure(conf, target);

            // Defines
            conf.ExportDefines.Add("ION_INPUT_SUPPORTS_XINPUT");
            conf.ExportDefines.Add("DIRECTINPUT_VERSION=0x0800");

            // Libs
            if (target.Platform == Platform.win32 || target.Platform == Platform.win64)
            {
                conf.LibraryFiles.Add("xinput.lib");
                conf.LibraryFiles.Add("dinput8.lib");
                conf.LibraryFiles.Add("dxguid.lib");
            }
        }
    }
}