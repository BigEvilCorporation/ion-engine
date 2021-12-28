using Sharpmake;

[Generate]
class IonEngine : IonLib
{
    public IonEngine() : base("engine")
    {
        AddTargets(Globals.IonTargetsDefault);
    }

    [Configure]
    public override void Configure(Project.Configuration conf, Target target)
    {
        base.Configure(conf, target);

            conf.AddPublicDependency<IonAudio>(target);
            conf.AddPublicDependency<IonCore>(target);
            conf.AddPublicDependency<IonGameKit>(target);
            conf.AddPublicDependency<IonGui>(target);
            conf.AddPublicDependency<IonInput>(target);
            conf.AddPublicDependency<IonMaths>(target);
            conf.AddPublicDependency<IonNetwork>(target);
            conf.AddPublicDependency<IonRenderer>(target);
            conf.AddPublicDependency<IonResource>(target);
            conf.AddPublicDependency<IonServices>(target);
            conf.AddPublicDependency<IonShaders>(target, DependencySetting.DefaultWithoutLinking);
    }
}