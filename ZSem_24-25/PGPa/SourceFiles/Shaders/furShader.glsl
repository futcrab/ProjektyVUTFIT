// Hair simulation PGPa project 24/25
// Peter Durica (xduric05) VUT FIT
// Hair simulation inspired by https://github.com/rcedermalm/Realtime-Rendering-of-Fur


#ifdef VERTEX_SHADER

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 TexCoord;

out vec2 vTexCoord;
out vec3 vNormal;
out float vVertexID;

void main(){
    vTexCoord = abs(TexCoord); // Using abs because in bunny object i used normals (which can be negative numbers) as texture coordinates
    vNormal = normalize(normal);
    vVertexID = gl_VertexID;

    gl_Position = vec4(position,1.f); // Just passing position, transformation is done in geometry shader
}
#endif

#ifdef CONTROL_SHADER

layout(vertices = 3) out;

uniform vec3 cameraPosition;
uniform float densityOfTesselation = 800.f;

in vec2 vTexCoord[];
in vec3 vNormal[];
in float vVertexID[];

out vec2 tcTexCoord[];
out vec3 tcNormal[];
out float tcVertexID[];

void main(){
    // Get normal of the triangle
    vec3 ac = vec3(gl_in[1].gl_Position - gl_in[0].gl_Position);
    vec3 bc = vec3(gl_in[2].gl_Position - gl_in[0].gl_Position);
    vec3 triangleNormal = cross(ac,bc);

    // Calculate how many tesselations are needed for area
    float area = 0.5f * length(triangleNormal);
    float numberOfTesselations = area * densityOfTesselation;

    gl_TessLevelInner[0] = numberOfTesselations;
    gl_TessLevelOuter[0] = numberOfTesselations;
    gl_TessLevelOuter[1] = numberOfTesselations;
    gl_TessLevelOuter[2] = numberOfTesselations;
    
    // Pass through
    tcTexCoord[gl_InvocationID] = vTexCoord[gl_InvocationID];
    tcNormal[gl_InvocationID] = vNormal[gl_InvocationID];
    tcVertexID[gl_InvocationID] = vVertexID[gl_InvocationID];
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
#endif

#ifdef EVALUATION_SHADER
layout (triangles, equal_spacing, ccw) in;

in vec2 tcTexCoord[];
in vec3 tcNormal[];
in float tcVertexID[];

out vec2 teTexCoord;
out vec3 teNormal;
out vec3 teVertexID;
out vec3 teTessCoords;

void main(){
    
    // Get position and texture coords of new tesselated vertices
    gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) +
                  (gl_TessCoord.y * gl_in[1].gl_Position) +
                  (gl_TessCoord.z * gl_in[2].gl_Position);

    teTexCoord = (gl_TessCoord.x * tcTexCoord[0]) +
                  (gl_TessCoord.y * tcTexCoord[1]) +
                  (gl_TessCoord.z * tcTexCoord[2]);

    teNormal = tcNormal[0];
    teTessCoords = gl_TessCoord;
    teVertexID = vec3(tcVertexID[0],tcVertexID[1],tcVertexID[2]);
}
#endif

#ifdef  GEOMETRY_SHADER

layout (triangles) in;
layout (line_strip, max_vertices = 12) out; // Hairs are being rendered as line strips

uniform mat4 model = mat4(1.f);
uniform mat4 view = mat4(1.f);
uniform mat4 proj = mat4(1.f);

// Hair uniforms
layout(binding=0)uniform sampler2D hairDataTexture;
layout(binding=1)uniform sampler2D randomDataTexture;
uniform float nOfHairSegments;
uniform float nOfVertices;
uniform float nOfDataVariablesPerMasterHair;
uniform float randomnessIntensity = 0.2f;

in vec2 teTexCoord[3];
in vec3 teNormal[3];
in vec3 teVertexID[3];
in vec3 teTessCoords[3];

out vec3 gPosition;
out vec3 gNormal;

// Offsets are used to centre the texture coordinates to the middle of the pixel (for better sampling)
float offsetWidth = (1.f / (nOfHairSegments * nOfDataVariablesPerMasterHair)) * 0.5f;
float offsetHeight = (1.f / nOfVertices) * 0.5f;

// Get position from hair data texture based on main vertex index and hair segment index
vec3 getPositionFromTexture(float vertexIndex, float hairIndex){
    vec2 hairStrandPos = vec2((hairIndex / nOfHairSegments) + offsetWidth, (vertexIndex / nOfVertices) + offsetHeight);
    return vec3(texture(hairDataTexture, hairStrandPos));
}

// Get interpolated position of the hair segment based on the main vertex index and hair segment index
vec3 getInterpolatedPosition(int index, int hairIndex){
    vec3 masterHairPos0 = getPositionFromTexture(teVertexID[index].x, hairIndex);
    vec3 masterHairPos1 = getPositionFromTexture(teVertexID[index].y, hairIndex);
    vec3 masterHairPos2 = getPositionFromTexture(teVertexID[index].z, hairIndex);

    // Interpolate position based on tesselated coordinates
    vec3 hairPos = teTessCoords[index].x * masterHairPos0 + teTessCoords[index].y * masterHairPos1 + teTessCoords[index].z * masterHairPos2;

    return hairPos;
}

void generateHairStrands(int index){
    
    vec3 lastPos = (gl_in[index].gl_Position).xyz; // Get position of the vertex in the triangle
    vec3 firstHairSegmentPos = getInterpolatedPosition(index, 0); // Get position of the first hair segment from texture

    // Now transform that position and pass variables to fragment shader
    gl_Position = proj*view*model*gl_in[index].gl_Position;
    gPosition = lastPos;
    gNormal = teNormal[index];
    EmitVertex();

    // Get randomness from texture
    vec3 randomness = vec3(texture(randomDataTexture, teTexCoord[index]));

    //Create other vertices for the strand
    for(int hairIndex = 1; hairIndex < nOfHairSegments; hairIndex++){
        vec3 hairPos = getInterpolatedPosition(index, hairIndex); // Get position of the hair segment from texture
        hairPos += randomness * randomnessIntensity; // Add randomness so it seems more realistic

        // Calculate position and pass variables to fragment shader
        gl_Position = proj * view * vec4(hairPos,1.f);
        gPosition = hairPos;
        gNormal = teNormal[index];
        EmitVertex();

        lastPos = hairPos;
    }

    // End primitive when all hair segments are generated
    EndPrimitive();
    
}

void main(){
    for(int i = 0; i < gl_in.length(); i++){
        generateHairStrands(i);
    }
}
#endif

#ifdef FRAGMENT_SHADER

uniform vec3 cameraPositionFrag;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 diffuseColor = vec3(90.f/255.f,56.f/255.f,37.f/255.f);

in vec3 gPosition;
in vec3 gNormal;

out vec4 fColor;

// Phong implementation from PGPa framework
vec3 phongLighting(
  vec3  position     ,
  vec3  normal       ,
  vec3  lightPosition,
  vec3  camera       ,
  vec3  lightColor   ,
  vec3  ambient      ,
  vec3  diffuseColor ,
  float shininess    ,
  float spec         ){

  vec3  L  = normalize(lightPosition-position);
  float Df = max(dot(normal,L),0);

  vec3  V  = normalize(camera-position);
  vec3  R  = -reflect(L,normal);
  float Sf = pow(max(dot(R,V),0),shininess);

  vec3 ambientLighting  = ambient*diffuseColor;
  vec3 diffuseLighting  = Df*diffuseColor*lightColor;
  vec3 specularLighting = Sf*vec3(1,1,1)*lightColor*spec;

  return ambientLighting + diffuseLighting + specularLighting;
}

void main(){

// Use phong lighting model to calculate final color based on hair color and light color
vec3 finalColor = phongLighting(
        gPosition,
        gNormal,
        lightPosition,
        cameraPositionFrag,
        lightColor,
        vec3(0.3,0.3,0.3),
        diffuseColor,
        150.f,
        0.1f
    );

    fColor = vec4(finalColor,1.f);
}
#endif