// final Shader
// Vertex Shader
#version 130

// Incoming per vertex... just the position
//in vec4 vVertex;

void main(void) 
{
    gl_Position    = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;

}
