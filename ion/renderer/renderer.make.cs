using Sharpmake;

[Generate]
class IonRenderer : IonLib
{
    public IonRenderer() : base("renderer")
    {
        AddTargets(Globals.IonTargetsDefault);
    }

    [Configure]
    public override void Configure(Project.Configuration conf, Target target)
    {
        base.Configure(conf, target);

        conf.AddPublicDependency<Dependencies.LibPNG>(target);
        conf.AddPublicDependency<Dependencies.LibZLib>(target);

        // TODO: only if not fixed renderer
        conf.AddPublicDependency<Dependencies.LibGLSL>(target);

        if (target.Platform == Platform.win32 || target.Platform == Platform.win64)
        {
            // OpenGL Core and SDL2 
            conf.AddPublicDependency<Dependencies.LibOpenGL>(target);
            conf.AddPublicDependency<Dependencies.LibSDL2>(target);
            conf.AddPublicDependency<Dependencies.LibSDL2Image>(target);

            // Freetype currently only supported on Windows
            conf.AddPublicDependency<Dependencies.LibFreetype>(target);
        }
        else  if (target.Platform == Platform.nx)
        {
            // OpenGLES
            conf.AddPublicDependency<Dependencies.LibOpenGLES>(target);
        }
    }
}
