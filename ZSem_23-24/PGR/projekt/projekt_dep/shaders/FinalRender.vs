#ifdef VERTEX_SHADER
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 inTextureCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inID;

out vec3 fragPos;
out vec3 normal;
out vec2 textureCoord;
out vec3 ID;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    fragPos = vec3(model * vec4(vertexPos, 1.0));

    normal = mat3(transpose(inverse(model))) * inNormal;

    textureCoord = inTextureCoord;

    ID = inID;

    gl_Position = projection * view * vec4(fragPos, 1.0);
}
#endif