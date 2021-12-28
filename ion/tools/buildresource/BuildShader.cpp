#include "BuildShader.h"
#include "core/string/String.h"

namespace ion
{
	namespace render
	{
		Shader* Shader::Create()
		{
			return new ShaderResource();
		}

		void Shader::RegisterSerialiseType(io::Archive& archive)
		{
			archive.RegisterPointerTypeStrict<Shader, ShaderResource>("ion::render::Shader");
		}
	}

	namespace build
	{
		int BuildShader(const std::string& outputFilename, const std::vector<ShaderProgram>& programs)
		{
			ion::render::Shader* shader = ion::render::Shader::Create();
			int numProgramsExported = 0;

			for (int i = 0; i < programs.size(); i++)
			{
				ion::render::Shader::Program* program = shader->GetProgram(programs[i].language, programs[i].programType);
				if (program)
				{
					debug::log << "Shader " << outputFilename << " of this type already has a program for language " << programs[i].language << ", ignoring" << debug::end;
				}
				else
				{
					ion::io::File programFile(programs[i].sourceFilename, ion::io::File::OpenMode::Read);
					if (programFile.IsOpen())
					{
						std::string code;
						code.resize(programFile.GetSize() + 1);
						programFile.Read(&code[0], programFile.GetSize());

						shader->SetProgram(ion::string::ToLower(programs[i].language), code, programs[i].entrypoint, programs[i].programType);

						numProgramsExported++;

						programFile.Close();
					}
					else
					{
						ion::debug::error << "Failed to open program " << programs[i].sourceFilename << " for shader " << outputFilename << ion::debug::end;
						return -1;
					}
				}
			}

			if (numProgramsExported)
			{
				ion::io::File file(outputFilename, ion::io::File::OpenMode::Write);
				if (file.IsOpen())
				{
					ion::io::Archive archive(file, ion::io::Archive::Direction::Out);
					ion::render::Shader::RegisterSerialiseType(archive);

					//Serialise pointer to shader
					archive.Serialise(shader);

					file.Close();

					debug::log << "Created shader " << outputFilename << " (" << numProgramsExported << " programs)" << debug::end;
				}
				else
				{
					debug::error << "Error opening shader file " << outputFilename << " for writing" << debug::end;
					return -1;
				}
			}
			else
			{
				debug::error << "Could not read any programs for shader " << outputFilename << ", shader not built" << debug::end;
				return -1;
			}

			return 0;
		}
	}
}