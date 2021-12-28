using Sharpmake;

[Generate]
class IonGameKit : IonLib
{
    public IonGameKit() : base("gamekit")
    {
        AddTargets(Globals.IonTargetsDefault);
    }

    [Configure]
    public override void Configure(Project.Configuration conf, Target target)
    {
        base.Configure(conf, target);
    }
}