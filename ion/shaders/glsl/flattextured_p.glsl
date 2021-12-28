#version 330 core

//Samplers
uniform sampler2D gDiffuseTexture;

//Fragment input
in vec2 texCoord;
in vec4 colour;

//Fragment output
out vec4 outputColour;

void main()
{
	outputColour = vec4(colour * texture(gDiffuseTexture, texCoord));
}
