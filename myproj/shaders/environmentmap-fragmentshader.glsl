#version 330 core

uniform mat4 myview_matrix;
uniform mat3 mynormal_matrix;
uniform mat4 mymodel_matrix;

in vec4 myvertex;
in vec3 mynormal;
in vec2 mytexturecoordinate;
 
out vec4 color;

uniform sampler2D tex;
uniform samplerCube cubetex;

uniform int totexture;

 
void main (void)
{   
	vec4 _mypos = myview_matrix * mymodel_matrix * myvertex;
	vec3 mypos = (_mypos.xyz) / _mypos.w;

	vec3 eyepos = vec3(0,0,0);

    vec3 normal = normalize(mynormal_matrix * mynormal);

	vec3 mypos_to_eyepos = normalize(eyepos - mypos);
	
	vec3 reflection = reflect(-mypos_to_eyepos, normal);
	color = texture(cubetex, mynormal);


	color.a = 1.0f;
}
