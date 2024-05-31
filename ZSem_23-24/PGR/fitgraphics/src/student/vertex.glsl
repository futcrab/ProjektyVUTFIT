#version 460 core
layout(location=0)in vec3 position;
layout(location=1)in vec2 tex_loc;

uniform mat4 view = mat4(1);
uniform mat4 proj = mat4(1);
	
uniform float lightCube_size = 0.1f;
uniform vec3 lightPos = vec3(1.f);

uniform int switcher = 0;

out vec2 Vcoord;

mat4 CreateTMatrix(){
	mat4 matrix = mat4(1);
	matrix[3][0] = lightPos[0];
	matrix[3][1] = lightPos[1];
	matrix[3][2] = lightPos[2];

	return matrix;
}
	

void main(){
	if(switcher == 0){
		gl_Position = proj * view * vec4(position,1);
		Vcoord = tex_loc;
	}
	else{
		mat4 CubeTMatrix = CreateTMatrix();
		gl_Position = proj * view * CubeTMatrix * vec4(position * lightCube_size,1);
		Vcoord = tex_loc;
	}
}