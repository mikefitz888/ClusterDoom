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

	float alpha_fac = 1.0;
	if( mask.r > healthProgress ){
		alpha_fac = clamp(1.0f - (mask.r-healthProgress)/0.1f, 0.0f, 1.0f);
	}

    out_Color = pass_Color*drawColour*base;
	out_Color.a = alpha_fac*base.a;
}
