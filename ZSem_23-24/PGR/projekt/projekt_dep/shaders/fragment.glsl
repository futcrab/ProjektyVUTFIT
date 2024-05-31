#ifdef FRAGMENT_SHADER
in vec2 Vcoord;
in vec3 Vnormal;
in vec3 VtriangleID;

layout(binding=0)uniform sampler2D tex;

out vec4 FColor;
	
void main(){
	FColor = texture(tex, Vcoord);
}
#endif