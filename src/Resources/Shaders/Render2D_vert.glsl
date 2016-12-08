#version 130

uniform mat4 matrixProjection;
uniform mat4 matrixWorld;


in vec4 in_Position;
in vec4 in_Colour;
in vec2 in_TextureCoord;
in vec3 in_Normals;

out vec4 pass_Color;
out vec2 pass_TextureCoord;

uniform vec4 colour;

void main(void) {
	gl_Position = matrixProjection * matrixWorld * in_Position;

	pass_Color = in_Colour;
	pass_TextureCoord = in_TextureCoord;
}
