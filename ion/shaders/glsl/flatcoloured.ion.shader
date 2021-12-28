�,�        ��?�;       񟵦/       �q�          ion::render::Shader�Ln��         ?�5,       񟵦        �q�          glsl.��         ?�5           .��      qD�I      񟵦=      �q�       !  #version 330 core

//Diffuse
uniform vec4 gDiffuseColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);

//Matrices
uniform mat4 gWorldViewProjectionMatrix;

//Vertex input
//Matches ion::render::VertexBuffer::ElementType
layout (location = 0) in vec4 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec4 in_colour;
layout (location = 3) in vec2 in_texCoord;

//Vertex output
out vec4 colour;

void main()
{
	gl_Position = gWorldViewProjectionMatrix * in_position;
	colour = in_colour * gDiffuseColour;
}
 ���,       񟵦        �q�          main?�5          .��       qD��       񟵦�       �q�       �   #version 330 core

//Fragment input
in vec4 colour;

//Fragment output
out vec4 outputColour;

void main()
{
	outputColour = colour;
}
 ���,       񟵦        �q�          main