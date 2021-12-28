using System.Collections.Generic;
using Sharpmake;
using System;
using System.Linq;
using System.IO;

public class BuildTexture : Sharpmake.Project.Configuration.CustomFileBuildStep
{
	public BuildTexture(string inputFile)
	{
		string textureName = Path.GetFileNameWithoutExtension(inputFile);
		string directory = Path.GetDirectoryName(inputFile);
		string outputFile = Path.Combine(directory, textureName);

		KeyInput = inputFile;
		Output = outputFile + ".ion.texture";
		Description = $"PNG texture {textureName}";
		Executable = "";
		ExecutableArguments = Globals.ResourceBuilder + $" texture {Output} {inputFile}";
	}

	public static void ConfigureTextures(Project project)
	{
		List<string> pngs = new List<string>(project.ResolvedSourceFiles.Where(file => file.EndsWith(".png", StringComparison.InvariantCultureIgnoreCase))).ToList();

		foreach (Sharpmake.Project.Configuration conf in project.Configurations)
		{
			foreach (string texture in pngs)
			{
				BuildTexture buildTask = new BuildTexture(texture);
				conf.CustomFileBuildSteps.Add(buildTask);
			}
		}
	}
}

public class BuildShader : Sharpmake.Project.Configuration.CustomFileBuildStep
{
	public BuildShader(string entryPoint, string sourceFile)
	{
		string name = Path.GetFileNameWithoutExtension(sourceFile);
		string directory = Path.GetDirectoryName(sourceFile);

		KeyInput = $"{directory}/{name}_v.glsl";
		Output = $"{directory}/{name}.ion.shader";
		Description = $"GLSL shader {name}";
		Executable = "";
		ExecutableArguments = Globals.ResourceBuilder + $" shader { Output} vshader glsl {directory}/{name}_v.glsl {entryPoint} pshader glsl {directory}/{name}_p.glsl {entryPoint}";
	}

	public static void ConfigureShaders(Project project)
	{
		// Find all _v.glsl and _p.glsl pairs
		List<string> vshaders = new List<string>(project.ResolvedSourceFiles.Where(file => file.EndsWith("_v.glsl", StringComparison.InvariantCultureIgnoreCase))).ToList();
		List<string> pshaders = new List<string>(project.ResolvedSourceFiles.Where(file => file.EndsWith("_p.glsl", StringComparison.InvariantCultureIgnoreCase))).ToList();

		vshaders = vshaders.Select(s => s.Replace("_v.glsl", "")).ToList();
		pshaders = pshaders.Select(s => s.Replace("_p.glsl", "")).ToList();
		List<string> combinedShaders = vshaders.Intersect(pshaders).ToList();

		foreach (Sharpmake.Project.Configuration conf in project.Configurations)
		{
			foreach (string shader in combinedShaders)
			{
				BuildShader compileTask = new BuildShader("main", Project.GetCapitalizedFile(shader));
				project.ResolvedSourceFiles.Add(compileTask.Output);
				conf.CustomFileBuildSteps.Add(compileTask);
			}
		}
	}
}