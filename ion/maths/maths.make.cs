using Sharpmake;

[Generate]
class IonMaths : IonLib
{
    public IonMaths() : base("maths")
    {
        AddTargets(Globals.IonTargetsDefault);
    }

    [Configure]
    public override void Configure(Project.Configuration conf, Target target)
    {
        base.Configure(conf, target);
    }
}