#version 130

uniform sampler2D texture_diffuse;
uniform sampler2D texture_mask;

uniform vec4 drawColour;
uniform float healthProgress;

in vec4 pass_Color;
in vec2 pass_TextureCoord;

out vec4 out_Color;

void main(void) {
    vec4 base = texture(texture_diffuse, pass_TextureCoord);
	vec4 mask = texture(texture_mask, pass_TextureCoord);

	if( mask.r > healthProgress ){
		discard;
	}

    out_Color = pass_Color*drawColour*base;
}
