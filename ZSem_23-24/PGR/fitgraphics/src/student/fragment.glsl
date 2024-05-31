#ifdef FRAGMENT_SHADER
in vec2 Vcoord;

layout(binding=0)uniform sampler2D tex;

out vec4 FColor;
	
void main(){
	FColor = texture(tex, Vcoord);
}
#endif