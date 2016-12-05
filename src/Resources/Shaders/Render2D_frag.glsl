#version 130

uniform sampler2D texture_diffuse;
uniform vec4 drawColour;

in vec4 pass_Color;
in vec2 pass_TextureCoord;

out vec4 out_Color;

void main(void) {
	vec4 base = texture(texture_diffuse, pass_TextureCoord);
	out_Color = pass_Color*drawColour*base;//pass_Color*vec4( 1.0,0.0,0.0,1.0)*base;//vec4(pass_TextureCoord.x, pass_TextureCoord.y, 0.0, 1.0);
}