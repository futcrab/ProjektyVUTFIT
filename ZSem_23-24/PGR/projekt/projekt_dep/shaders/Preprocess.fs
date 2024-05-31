#ifdef FRAGMENT_SHADER
layout (location = 0) out vec3 worldspacePosition;
layout (location = 1) out vec3 worldspaceNormal;
layout (location = 2) out vec3 triangleID;
layout (location = 3) out vec3 interpolatedUV;

in vec3 fragPos;
in vec3 normal;
in vec2 textureCoord;
in vec3 ID;

void main() {

    worldspacePosition = fragPos;
    
    worldspaceNormal = normalize(normal);

    triangleID = ID;

    interpolatedUV = vec3(textureCoord, 0.0);

}
#endif