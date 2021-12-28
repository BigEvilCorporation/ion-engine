using Sharpmake;

[Generate]
class IonCore : IonLib
{
    public IonCore() : base("core")
    {
        AddTargets(Globals.IonTargetsDefault);
    }

    [Configure]
    public override void Configure(Project.Configuration conf, Target target)
    {
        base.Configure(conf, target);

        // Additional platform dependencies
        if(target.Platform == Platform.nx)
        {
            conf.LibraryFiles.Add("nn_profiler");
        }
    }
}