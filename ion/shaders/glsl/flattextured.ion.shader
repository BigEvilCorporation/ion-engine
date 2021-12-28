�,�        ��?�;       񟵦/       �q�          ion::render::Shader�Ln��         ?�5,       񟵦        �q�          glsl.��         ?�5           .��      qD��      񟵦�      �q�       �  #version 330 core

//Diffuse
uniform vec4 gDiffuseColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);

//Matrices
uniform mat4 gWorldViewProjectionMatrix;

//UV scroll
uniform vec2 gUVScroll = vec2(0.0f, 0.0f);

//Vertex input
//Matches ion::render::VertexBuffer::ElementType
layout (location = 0) in vec4 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec4 in_colour;
layout (location = 3) in vec2 in_texCoord;

//Vertex output
out vec2 texCoord;
out vec4 colour;

void main()
{
	gl_Position = gWorldViewProjectionMatrix * in_position;
	texCoord = in_texCoord + gUVScroll;
	colour = in_colour * gDiffuseColour;
}
 ���,       񟵦        �q�          main?�5          .�d      qD�,      񟵦       �q�         #version 330 core

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
 ���,       񟵦        �q�          main