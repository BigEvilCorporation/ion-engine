using Sharpmake;

[Generate]
class IonNetwork : IonLib
{
    public IonNetwork() : base("network")
    {
        AddTargets(Globals.IonTargetsDefault);
    }

    [Configure]
    public override void Configure(Project.Configuration conf, Target target)
    {
        base.Configure(conf, target);
    }
}