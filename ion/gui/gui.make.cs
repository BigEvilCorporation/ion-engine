using Sharpmake;

[Generate]
class IonGui : IonLib
{
    public IonGui() : base("gui")
    {
        AddTargets(Globals.IonTargetsDefault);
    }

    [Configure]
    public override void Configure(Project.Configuration conf, Target target)
    {
        base.Configure(conf, target);

        conf.AddPublicDependency<IonRenderer>(target);
        conf.AddPublicDependency<Dependencies.LibIMGUI>(target);
    }
}