using Sharpmake;

[Generate]
class IonResource : IonLib
{
    public IonResource() : base("resource")
    {
        AddTargets(Globals.IonTargetsDefault);
    }

    [Configure]
    public override void Configure(Project.Configuration conf, Target target)
    {
        base.Configure(conf, target);
    }
}