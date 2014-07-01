// final Shader
// Fragment Shader
#version 130

out vec4 vFragColor;

uniform sampler2D tex_water;
uniform sampler2D tex_ice;

void main(void)
{ 
	vec4 color = vec4(0, 0, 0, 0);
	vec4 color_interface = vec4( 0.3, 0.5, 0.6, 1.0);
	vec4 color_water = texture2D(tex_water, gl_TexCoord[0].st);
	vec4 color_ice = texture2D(tex_ice, gl_TexCoord[0].st);
	//if(color_water != color && color_ice != color)
	//	vFragColor = color_interface;
	//else
		vFragColor = texture2D(tex_water, gl_TexCoord[0].st) + texture2D(tex_ice, gl_TexCoord[0].st);
   // + texture2D(tex_ice, gl_TexCoord[0].st);
    //vFragColor = color;
}
    