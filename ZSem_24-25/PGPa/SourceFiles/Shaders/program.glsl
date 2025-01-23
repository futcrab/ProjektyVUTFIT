// Hair simulation PGPa project 24/25
// Peter Durica (xduric05) VUT FIT
// Hair simulation inspired by https://github.com/rcedermalm/Realtime-Rendering-of-Fur


// Simple shader that calculates position and passes color to fragment shader
#ifdef VERTEX_SHADER

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 TexCoord;

uniform mat4 view = mat4(1.f);
uniform mat4 proj = mat4(1.f);
uniform mat4 model = mat4(1.f);

uniform vec3 color;

out vec3 vColor;
void main(){
    vColor = color;

    gl_Position = proj*view*model*vec4(position,1.f);
}
#endif

#ifdef FRAGMENT_SHADER
in vec3 vColor;

out vec4 fColor;

void main(){
    fColor = vec4(vColor,1);
}
#endif