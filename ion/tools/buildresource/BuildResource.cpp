///////////////////////////////////////////////////
// File:		BuildResource.h
// Date:		15th February 2014
// Authors:		Matt Phillips
// Description:	Resource build tool
///////////////////////////////////////////////////

#pragma once

#include "core/containers/Queue.h"
#include "core/debug/Debug.h"
#include "core/string/String.h"
#include "core/io/File.h"

#include "tools/buildresource/BuildTexture.h"
#include "tools/buildresource/BuildShader.h"

#include <string>
#include <vector>
#include <locale>
#include <algorithm>
#include <iostream>

const int sMaxTokens = 32;

void ShowHelp()
{
	std::cout << "TODO: help" << std::endl;
}

bool PopToken(ion::Queue<std::string, sMaxTokens>& tokens, std::string& poppedToken, const std::string& error, bool lowerCase = false)
{
	if(tokens.IsEmpty())
	{
		if (error.size() > 0)
		{
			ion::debug::error << error << ion::debug::end;
			ShowHelp();
		}
		
		return false;
	}

	poppedToken = tokens.Pop();

	if(lowerCase)
		std::transform(poppedToken.begin(), poppedToken.end(), poppedToken.begin(), ::tolower);

	return true;
}

int ParseCmdTexture(ion::Queue<std::string, sMaxTokens>& tokens)
{
	std::string outputFilename;
	std::string textureFile;

	if (!PopToken(tokens, outputFilename, "Missing texture output filename"))
		return -1;
	if (!PopToken(tokens, textureFile, "Missing texture input filename"))
		return -1;

	int dot = textureFile.find_last_of('.') + 1;
	int end = textureFile.size() - dot;
	std::string fileExt = textureFile.substr(dot, end);
	std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::tolower);

	ion::build::TextureFiletype filetype = ion::build::TextureFiletype::BMP;

	if(fileExt == "bmp")
		filetype = ion::build::TextureFiletype::BMP;
	else if(fileExt == "png")
		filetype = ion::build::TextureFiletype::PNG;
	
	return ion::build::BuildTexture(outputFilename, textureFile, filetype);
}

int ParseCmdShader(ion::Queue<std::string, sMaxTokens>& tokens)
{
	std::vector<ion::build::ShaderProgram> shaderPrograms;
	std::string outputFilename;
	std::string programType;

	if (!PopToken(tokens, outputFilename, "Missing shader output filename"))
		return -1;

	while (PopToken(tokens, programType, "", true))
	{
		if (programType == "vshader")
		{
			ion::build::ShaderProgram program;
			program.programType = ion::render::Shader::ProgramType::Vertex;

			if (!PopToken(tokens, program.language, "Missing vertex shader language"))
				return -1;
			if (!PopToken(tokens, program.sourceFilename, "Missing vertex shader source"))
				return -1;
			if (!PopToken(tokens, program.entrypoint, "Missing vertex shader entry point"))
				return -1;

			shaderPrograms.push_back(program);
		}
		else if (programType == "pshader")
		{
			ion::build::ShaderProgram program;
			program.programType = ion::render::Shader::ProgramType::Fragment;

			if (!PopToken(tokens, program.language, "Missing fragment shader language"))
				return -1;
			if (!PopToken(tokens, program.sourceFilename, "Missing fragment shader source"))
				return -1;
			if (!PopToken(tokens, program.entrypoint, "Missing fragment shader entry point"))
				return -1;

			shaderPrograms.push_back(program);
		}
		else
		{
			ion::debug::error << "Unrecognised program type " << programType << ion::debug::end;
			return -1;
		}
	}

	if (shaderPrograms.size())
		return ion::build::BuildShader(outputFilename, shaderPrograms);

	ion::debug::error << "No shaders built" << ion::debug::end;
	return -1;
}

int main(int numargs, char** args)
{
	ion::Queue<std::string, sMaxTokens> tokens;

	for (int i = 1; i < numargs; i++)
		tokens.Push(std::string(args[i]));

	std::string resourceType ;

	if (!PopToken(tokens, resourceType, "Missing resource type", true))
		return -1;

	if (resourceType == "texture")
		return ParseCmdTexture(tokens);
	else if (resourceType == "shader")
		return ParseCmdShader(tokens);

	ion::debug::error << "Unrecognised resource type " << resourceType << ion::debug::end;

	return -1;
}
