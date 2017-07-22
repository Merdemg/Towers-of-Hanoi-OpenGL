#version 150 



in   vec4 vPosition;
in   vec3 vNormal;
in	vec4 vColor;

// output values that will be interpretated per-fragment
out  vec3 fN;
out  vec3 fE;
out  vec3 fL;
out vec4 myColor;

uniform mat4 ModelView;
uniform vec4 LightPosition;
uniform mat4 projection_matrix;
uniform mat4 camera_matrix;
uniform mat4 model_matrix;

void main()
{
	myColor = vColor;
	fN = vNormal;
    fE = (camera_matrix*vPosition).xyz;
    fL = LightPosition.xyz;
    
    if( LightPosition.w != 0.0 ) {
	fL = LightPosition.xyz - vPosition.xyz;
    }

    gl_Position = projection_matrix * camera_matrix * model_matrix * vPosition;
}
