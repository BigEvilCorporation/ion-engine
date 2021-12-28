using Sharpmake;
using System.IO;

[Generate]
class IonBeehive : IonLib
{
    public enum ExportPlugin
    {
        Tanglewood,
        Luminary
    }

    public const ExportPlugin exportPlugin = ExportPlugin.Tanglewood;

    public IonBeehive() : base("beehive")
    {
        AddTargets(Globals.IonTargetsDefault);
    }

    [Configure]
    public override void Configure(Project.Configuration conf, Target target)
    {
        base.Configure(conf, target);

        // Configure features
#if ION_SUPPORT_PLUGINS
        switch (exportPlugin)
        {
            case ExportPlugin.Luminary:
            {
                conf.AddPublicDependency<Luminary>(target);

                conf.Defines.Add("BEEHIVE_PLUGIN_LUMINARY=1");
                conf.Defines.Add("BEEHIVE_FIXED_STAMP_MODE=1");
                conf.Defines.Add("BEEHIVE_LEAN_UI=1");
                conf.Defines.Add("BEEHIVE_GAMEOBJ_ORIGIN_CENTRE=1");

                conf.ExportDefines.Add("BEEHIVE_PLUGIN_LUMINARY=1");
                conf.ExportDefines.Add("BEEHIVE_FIXED_STAMP_MODE=1");
                conf.ExportDefines.Add("BEEHIVE_LEAN_UI=1");
                conf.ExportDefines.Add("BEEHIVE_GAMEOBJ_ORIGIN_CENTRE=1");

                // Requires Luminary checkout at ../luminary
                conf.IncludePaths.Add(Path.Combine(Globals.RootDir, "../LUMINARY/TOOLS/BeehivePlugins"));
                break;
            }
        }
#endif
    }
}