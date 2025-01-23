// Hair simulation PGPa project 24/25
// Peter Durica (xduric05) VUT FIT
// Hair simulation inspired by https://github.com/rcedermalm/Realtime-Rendering-of-Fur

#include<glm/geometric.hpp>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<SDL.h>
#include<Vars/Vars.h>
#include<geGL/geGL.h>
#include<geGL/StaticCalls.h>
#include<imguiDormon/imgui.h>
#include<imguiVars/addVarsLimits.h>
#include<framework/FunctionPrologue.h>
#include<framework/methodRegister.hpp>
#include<framework/makeProgram.hpp>

#include<framework/bunny.hpp>
#include<random>
#include<fstream>
#include<chrono>

#ifndef M_PI
#define M_PI 3.1415926536
#endif // M_PI

#define PROJECT_DIR std::string(CMAKE_ROOT_DIR) + "/src/student"

using namespace ge::gl;
using namespace std;

namespace student::project{

    // Pointers for programs
    shared_ptr<Program    >program;
    shared_ptr<Program    >furShader;
    shared_ptr<Program    >furCompute;
    
    // Pointers for objects
    shared_ptr<VertexArray>vao;
    
    // Box object
    shared_ptr<VertexArray>BoxVao;
    GLfloat* BoxVertexArray;
    GLuint* BoxIndexArray;

    // Sphere object
    shared_ptr<VertexArray>SphereVao;
    GLfloat* SphereVertexArray;
    GLuint* SphereIndexArray;
    
    //Stanford bunny
    shared_ptr<VertexArray>BunnyVao;

    unsigned ntris = 0; // number of indices
    unsigned nverts = 0; // number of vertices

    // Camera matrices
    glm::mat4 proj = glm::mat4(1.f);
    glm::mat4 view = glm::mat4(1.f);
    glm::mat4 model = glm::mat4(1.f);

    //Hair variables
    const int nOfHairSegments = 12; // Hardcoded in compute and geometry shader
    float hairSegmentLength = 0.05f; //0.05f
    const int nOfDataVariablesForMasterHair = 1;
    int nOfMasterHairs;

    //Wind variables 
    float windMagnitude = 1.f;
    glm::vec4 windDirection = { 0.f, -1.f, 1.f, 0.f };

    // Light variables
    glm::vec3 lightColor = { 1.f, 1.f, 1.f };
    glm::vec3 lightPos = glm::vec3(1.5f);
    glm::mat4 lightModel = glm::mat4(1.f);

    //Time variables
    float currTime = 0.f;

    //Hair pointers
    GLfloat* hairData;
    GLfloat* BoxHairData;
    GLfloat* SphereHairData;
    GLfloat* BunnyHairData;

    // Textures to store hair positions in different frames
    GLuint hairDataTextureID_rest, hairDataTextureID_last, hairDataTextureID_current, hairDataTextureID_simulated;

    // Texture for random values to insert a small randomness to hair strands
    GLuint randomDataTextureID;

    // Function to read files (shaders) into memory
    std::string readFileToString(const std::string& filename) {
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return "";
        }

        std::string content((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());

        file.close();

        return content;
    }

    // Get camera position from orbit camera variables
    glm::vec3 calculateCameraPosition(vars::Vars& vars) {
        float angleX = vars.getFloat("method.orbit.angleX");
        float angleY = vars.getFloat("method.orbit.angleY");
        float distance = vars.getFloat("method.orbit.distance");

        float x = -1.f * distance * glm::cos(angleX) * glm::sin(angleY);
        float y = distance * glm::sin(angleX);
        float z = distance * glm::cos(angleX) * glm::cos(angleY);

        return glm::vec3(x, y, z);
    }

    // Load stanford bunny from PGPa framework
    void createBunny(vars::Vars& vars) {
        BunnyVao = make_shared<VertexArray>();
        
        FUNCTION_PROLOGUE("method.bunny");
        int ntris = sizeof(bunnyIndices) / sizeof(VertexIndex);
        int nverts = sizeof(bunnyVertices) / sizeof(BunnyVertex);

        vars.addOrGetInt32("method.bunny.nverts") = nverts;
        vars.addOrGetInt32("method.bunny.ntris") = ntris;

        // Create buffers on GPU and save object values
        auto vbo = vars.reCreate<Buffer>("method.bunny.vbo", sizeof(bunnyVertices), bunnyVertices);
        auto ebo = vars.reCreate<Buffer>("method.bunny.ebo", sizeof(bunnyIndices), bunnyIndices);

        // Bind those values to VAO
        BunnyVao->addAttrib(vbo, 0, 3, GL_FLOAT, sizeof(BunnyVertex), 0);
        BunnyVao->addAttrib(vbo, 1, 3, GL_FLOAT, sizeof(BunnyVertex), sizeof(BunnyVertex::position));
        BunnyVao->addAttrib(vbo, 2, 2, GL_FLOAT, sizeof(BunnyVertex), sizeof(BunnyVertex::position)); // random values for TexCoords (only used to retrieve randomly generated data from texture)

        BunnyVao->addElementBuffer(ebo);
    }


    /*Creation of sphere inspired by: Stefan Gustavson(stegu@itn.liu.se) 2014.
    //* This code is in the public domain.
    */
    void createSphere(float radius, int segments, vars::Vars& vars) {
        SphereVao = make_shared<VertexArray>();

        int i, j, base, i0;
        float x, y, z, R;
        double theta, phi;
        int vsegs, hsegs;
        int stride = 8;

        vsegs = segments;
        if (vsegs < 2) vsegs = 2;
        hsegs = vsegs * 2;
        int nverts = 1 + (vsegs - 1) * (hsegs + 1) + 1; // top + middle + bottom
        int ntris = hsegs + (vsegs - 2) * hsegs * 2 + hsegs; // top + middle + bottom
        SphereVertexArray = new float[nverts * 8];
        SphereIndexArray = new GLuint[ntris * 3];

        // The vertex array: 3D xyz, 3D normal, 2D st (8 floats per vertex)
    // First vertex: top pole (+z is "up" in object local coords)
        SphereVertexArray[0] = 0.0f;
        SphereVertexArray[1] = 0.0f;
        SphereVertexArray[2] = radius;
        SphereVertexArray[3] = 0.0f;
        SphereVertexArray[4] = 0.0f;
        SphereVertexArray[5] = 1.0f;
        SphereVertexArray[6] = 0.5f;
        SphereVertexArray[7] = 1.0f;
        // Last vertex: bottom pole
        base = (nverts - 1) * stride;
        SphereVertexArray[base] = 0.0f;
        SphereVertexArray[base + 1] = 0.0f;
        SphereVertexArray[base + 2] = -radius;
        SphereVertexArray[base + 3] = 0.0f;
        SphereVertexArray[base + 4] = 0.0f;
        SphereVertexArray[base + 5] = -1.0f;
        SphereVertexArray[base + 6] = 0.5f;
        SphereVertexArray[base + 7] = 0.0f;
        // All other vertices:
        // vsegs-1 latitude rings of hsegs+1 vertices each
        // (duplicates at texture seam s=0 / s=1)
        for (j = 0; j < vsegs - 1; j++) { // vsegs-1 latitude rings of vertices
            theta = (double)(j + 1) / vsegs * M_PI;
            z = (float) cos(theta);
            R = (float) sin(theta);
            for (i = 0; i <= hsegs; i++) { // hsegs+1 vertices in each ring (duplicate for texcoords)
                phi = (double)i / hsegs * 2.0 * M_PI;
                x = R * ((float) cos(phi));
                y = R * ((float) sin(phi));
                base = (1 + j * (hsegs + 1) + i) * stride;
                SphereVertexArray[base] = radius * x;
                SphereVertexArray[base + 1] = radius * y;
                SphereVertexArray[base + 2] = radius * z;
                SphereVertexArray[base + 3] = x;
                SphereVertexArray[base + 4] = y;
                SphereVertexArray[base + 5] = z;
                SphereVertexArray[base + 6] = (float)i / hsegs;
                SphereVertexArray[base + 7] = 1.0f - (float)(j + 1) / vsegs;
            }
        }

        // The index array: triplets of integers, one for each triangle
        // Top cap
        for (i = 0; i < hsegs; i++) {
            SphereIndexArray[3 * i] = 0;
            SphereIndexArray[3 * i + 1] = 1 + i;
            SphereIndexArray[3 * i + 2] = 2 + i;
        }
        // Middle part (possibly empty if vsegs=2)
        for (j = 0; j < vsegs - 2; j++) {
            for (i = 0; i < hsegs; i++) {
                base = 3 * (hsegs + 2 * (j * hsegs + i));
                i0 = 1 + j * (hsegs + 1) + i;
                SphereIndexArray[base] = i0;
                SphereIndexArray[base + 1] = i0 + hsegs + 1;
                SphereIndexArray[base + 2] = i0 + 1;
                SphereIndexArray[base + 3] = i0 + 1;
                SphereIndexArray[base + 4] = i0 + hsegs + 1;
                SphereIndexArray[base + 5] = i0 + hsegs + 2;
            }
        }
        // Bottom cap
        base = 3 * (hsegs + 2 * (vsegs - 2) * hsegs);
        for (i = 0; i < hsegs; i++) {
            SphereIndexArray[base + 3 * i] = nverts - 1;
            SphereIndexArray[base + 3 * i + 1] = nverts - 2 - i;
            SphereIndexArray[base + 3 * i + 2] = nverts - 3 - i;
        }

        vars.addOrGetInt32("method.sphere.nverts") = nverts;
        vars.addOrGetInt32("method.sphere.ntris") = ntris;

        // Create buffers on GPU and save object values
        auto SphereVBO = vars.reCreate<Buffer>("method.sphere.vbo", nverts * 8 * sizeof(float), SphereVertexArray);
        auto SphereEBO = vars.reCreate<Buffer>("method.sphere.ebo", ntris * 3 * sizeof(GLuint), SphereIndexArray);

        // Bind those values to VAO
        SphereVao->addAttrib(SphereVBO, 0, 3, GL_FLOAT, 8 * sizeof(GLfloat), 0);
        SphereVao->addAttrib(SphereVBO, 1, 3, GL_FLOAT, 8 * sizeof(GLfloat), 3 * sizeof(GLfloat));
        SphereVao->addAttrib(SphereVBO, 2, 2, GL_FLOAT, 8 * sizeof(GLfloat), 6 * sizeof(GLfloat));

        SphereVao->addElementBuffer(SphereEBO);
    }

    void createBox(float xsize, float ysize, float zsize, vars::Vars& vars) {
        BoxVao = make_shared<VertexArray>();

        const GLfloat vertex_array_data[] = {

            xsize, ysize, zsize,   1.0f, 0.0f, 0.0f,   2 / 3.0f, 1 / 2.0f, // Vertex 0
            xsize, ysize, zsize,   0.0f, 1.0f, 0.0f,   2 / 3.0f, 1 / 2.0f, // Vertex 0
            xsize, ysize, zsize,   0.0f, 0.0f, 1.0f,   2 / 3.0f, 1 / 2.0f, // Vertex 0

            xsize, ysize, -zsize,   1.0f, 0.0f, 0.0f,   1.0f, 1 / 2.0f, // Vertex 1
            xsize, ysize, -zsize,   0.0f, 1.0f, 0.0f,   2 / 3.0f, 1.0f, // Vertex 1
            xsize, ysize, -zsize,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f, // Vertex 1

            -xsize,  ysize, -zsize,   -1.0f, 0.0f, 0.0f,   0.0f, 1 / 2.0f,  // Vertex 2
            -xsize,  ysize, -zsize,   0.0f, 1.0f, 0.0f,   1 / 3.0f, 1.0f,  // Vertex 2
            -xsize,  ysize, -zsize,   0.0f, 0.0f, -1.0f,   1 / 3.0f, 1.0f,  // Vertex 2

            -xsize,  ysize, zsize,   -1.0f, 0.0f, 0.0f,   1 / 3.0f, 1 / 2.0f,  // Vertex 3
            -xsize,  ysize, zsize,   0.0f, 1.0f, 0.0f,   1 / 3.0f, 1 / 2.0f,  // Vertex 3
            -xsize,  ysize, zsize,   0.0f, 0.0f, 1.0f,   1 / 3.0f, 1 / 2.0f,  // Vertex 3

            xsize, -ysize,  zsize,   1.0f, 0.0f, 0.0f,   2 / 3.0f, 0.0f, // Vertex 4
            xsize, -ysize,  zsize,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f, // Vertex 4
            xsize, -ysize,  zsize,   0.0f, 0.0f, 1.0f,   2 / 3.0f, 0.0f, // Vertex 4

            xsize, -ysize,  -zsize,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // Vertex 5
            xsize, -ysize,  -zsize,   0.0f, -1.0f, 0.0f,   1.0f, 1 / 2.0f, // Vertex 5
            xsize, -ysize,  -zsize,   0.0f, 0.0f, -1.0f,   0.0f, 1 / 2.0f, // Vertex 5

            -xsize,  -ysize,  -zsize,   -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // Vertex 6
            -xsize,  -ysize,  -zsize,   0.0f, -1.0f, 0.0f,   2 / 3.0f, 1 / 2.0f,  // Vertex 6
            -xsize,  -ysize,  -zsize,   0.0f, 0.0f, -1.0f,   1 / 3.0f, 1 / 2.0f,  // Vertex 6

            -xsize,  -ysize,  zsize,   -1.0f, 0.0f, 0.0f,   1 / 3.0f, 0.0f,  // Vertex 7
            -xsize,  -ysize,  zsize,   0.0f, -1.0f, 0.0f,   2 / 3.0f, 1.0f,  // Vertex 7
            -xsize,  -ysize,  zsize,   0.0f, 0.0f, 1.0f,   1 / 3.0f, 0.0f  // Vertex 7
        };
        const GLuint index_array_data[] = {
                1, 4, 10,
                4, 7, 10,
                6, 18, 9,
                9, 18, 21,
                2, 11, 23,
                2, 23, 14,
                0, 12, 3,
                3, 12, 15,
                5, 17, 8,
                8, 17, 20,
                13, 22, 16,
                16, 22, 19
        };

        int nverts = 24;
        int ntris = 12;
        vars.addOrGetInt32("method.box.nverts") = nverts;
        vars.addOrGetInt32("method.box.ntris") = ntris;

        BoxVertexArray = new GLfloat[8 * nverts];
        BoxIndexArray = new GLuint[3 * ntris];

        // Transform values into allocated memory
        for (int i = 0; i < 8 * nverts; i++) {
            BoxVertexArray[i] = vertex_array_data[i];
        }
        for (int i = 0; i < 3 * ntris; i++) {
            BoxIndexArray[i] = index_array_data[i];
        }

        // Create buffers on GPU and save object values
        auto BoxVBO = vars.reCreate<Buffer>("method.box.vbo", sizeof(vertex_array_data), vertex_array_data);
        auto BoxEBO = vars.reCreate<Buffer>("method.box.ebo", sizeof(index_array_data), index_array_data);

        // Bind those values to VAO
        BoxVao->addAttrib(BoxVBO, 0, 3, GL_FLOAT, 8 * sizeof(GLfloat), 0);
        BoxVao->addAttrib(BoxVBO, 1, 3, GL_FLOAT, 8 * sizeof(GLfloat), 3 * sizeof(GLfloat));
        BoxVao->addAttrib(BoxVBO, 2, 2, GL_FLOAT, 8 * sizeof(GLfloat), 6 * sizeof(GLfloat));

        BoxVao->addElementBuffer(BoxEBO);
    }


    // Function from PGPa camera framework
    void computeProjectionMatrix(vars::Vars& vars) {
        auto width = vars.getUint32("event.resizeX");
        auto height = vars.getUint32("event.resizeY");
        auto near = vars.getFloat("method.camera.near");
        auto far = vars.getFloat("method.camera.far");

        float aspect = static_cast<float>(width) / static_cast<float>(height);
        proj = glm::perspective(glm::half_pi<float>(), aspect, near, far);
    }

    // Function from PGPa camera framework
    void computeViewMatrix(vars::Vars& vars) {
        auto angleX = vars.getFloat("method.orbit.angleX");
        auto angleY = vars.getFloat("method.orbit.angleY");
        auto distance = vars.getFloat("method.orbit.distance");
        auto cameraPosition = vars.get<glm::vec3>("method.orbit.cameraPosition");
        
        view =
            glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -distance)) *
            glm::rotate(glm::mat4(1.f), angleX, glm::vec3(1.f, 0.f, 0.f)) *
            glm::rotate(glm::mat4(1.f), angleY, glm::vec3(0.f, 1.f, 0.f));

        // Calculate and save camera position
        auto pos = calculateCameraPosition(vars);
        cameraPosition->x = pos.x;
        cameraPosition->y = pos.y;
        cameraPosition->z = pos.z;
    }

    // Create Hair for every vertex in object
    GLfloat* createMasterHairs(float* vertexArray, int nOfMasterHairs) {
        int amountOfDataPerMasterHair = nOfHairSegments * 4 * nOfDataVariablesForMasterHair; // 12 hair segments for each hair * 1 data variable (position) * 4 values (xyzw) for position
        GLfloat* hairData = new GLfloat[nOfMasterHairs * amountOfDataPerMasterHair]; // allocate buffer with calculated size for each object

        int masterHairIndex = 0;
        int stride = 8; // use stride 8 since we have this structure in buffer (3 * pos, 3 * normal, 2 * TexCoords)
        for (int i = 0; i < nOfMasterHairs * stride; i += stride) {
            // Save root pos and normal
            glm::vec4 rootPos = glm::vec4(vertexArray[i], vertexArray[i + 1], vertexArray[i + 2], 1.f);
            rootPos = model * rootPos;
            glm::vec4 rootNormal = glm::vec4(vertexArray[i+3], vertexArray[i+4], vertexArray[i+5], 0.f);

            hairData[masterHairIndex++] = rootPos.x;
            hairData[masterHairIndex++] = rootPos.y;
            hairData[masterHairIndex++] = rootPos.z;
            hairData[masterHairIndex++] = rootPos.w;

            for (int hairSegment = 0; hairSegment < nOfHairSegments - 1; hairSegment++) {
                // For every hair segment calculate their position based on normal and its index
                glm::vec4 newPos = rootPos + hairSegment * hairSegmentLength * rootNormal;
                hairData[masterHairIndex++] = newPos.x;
                hairData[masterHairIndex++] = newPos.y;
                hairData[masterHairIndex++] = newPos.z;
                hairData[masterHairIndex++] = newPos.w;
            }
        }
        return hairData;
    }

    // Similiar to previous function but adjusted for special type used in bunny object
    GLfloat* createMasterHairsforBunny(const BunnyVertex* vertexArray, int nOfMasterHairs) {
        int amountOfDataPerMasterHair = nOfHairSegments * 4 * nOfDataVariablesForMasterHair;
        GLfloat* hairData = new GLfloat[nOfMasterHairs * amountOfDataPerMasterHair];

        int masterHairIndex = 0;
        for (int i = 0; i < nOfMasterHairs; i++) {
            glm::vec4 rootPos = glm::vec4(vertexArray[i].position[0], vertexArray[i].position[1], vertexArray[i].position[2], 1.f);
            rootPos = model * rootPos;
            glm::vec4 rootNormal = glm::vec4(vertexArray[i].normal[0], vertexArray[i].normal[1], vertexArray[i].normal[2], 0.f);

            hairData[masterHairIndex++] = rootPos.x;
            hairData[masterHairIndex++] = rootPos.y;
            hairData[masterHairIndex++] = rootPos.z;
            hairData[masterHairIndex++] = rootPos.w;

            for (int hairSegment = 0; hairSegment < nOfHairSegments - 1; hairSegment++) {
                glm::vec4 newPos = rootPos + hairSegment * hairSegmentLength * rootNormal;
                hairData[masterHairIndex++] = newPos.x;
                hairData[masterHairIndex++] = newPos.y;
                hairData[masterHairIndex++] = newPos.z;
                hairData[masterHairIndex++] = newPos.w;
            }
        }
        return hairData;
    }

    // Create texture on GPU and save hair data into it
    GLuint generateTextureFromHairData(GLfloat* hairData, int nOfMasterHairs) {
        GLuint hairDataTextureID;

        glCreateTextures(GL_TEXTURE_2D, 1, &hairDataTextureID);
        glTextureImage2DEXT(hairDataTextureID, GL_TEXTURE_2D, 0, GL_RGBA16F, nOfHairSegments * nOfDataVariablesForMasterHair, nOfMasterHairs, 0, GL_RGBA, GL_FLOAT, hairData);

        glTextureParameteri(hairDataTextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(hairDataTextureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        return hairDataTextureID;
    }

    // Generate random texture values using normal distribution
    GLuint createRandomness() {
        GLfloat* randomData = new GLfloat[2048 * 2048 * 3];

        std::random_device rd;
        std::mt19937* gen = new std::mt19937(rd());
        std::uniform_real_distribution<float>* dis = new std::uniform_real_distribution<float>(-1.f, 1.f);

        // for every in texture generate new value
        for (int i = 0; i < 2048 * 2048 * 3; i++) {
            randomData[i] = (*dis)(*gen) * 0.3f;
        }

        delete gen;
        delete dis;

        // Create texture on GPU and save random values inside it
        GLuint randomDataTextureID;
        glCreateTextures(GL_TEXTURE_2D, 1, &randomDataTextureID);

        glTextureImage2DEXT(randomDataTextureID, GL_TEXTURE_2D, 0, GL_RGBA16F, 2048, 2048, 0, GL_RGB, GL_FLOAT, randomData);

        glTextureParameteri(randomDataTextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(randomDataTextureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // free random data on CPU since they're already saved on GPU
        delete[] randomData;

        return randomDataTextureID;
    }

void onKeyDown(vars::Vars&vars){
  ///which key was pressed
  auto key = vars.getInt32("event.key");

  //Camera variables
  auto sensitivity = vars.getFloat("method.sensitivity");
  auto& angleX = vars.getFloat("method.orbit.angleX");
  auto& angleY = vars.getFloat("method.orbit.angleY");

  if (key == SDLK_a)angleY += sensitivity;
  if (key == SDLK_d)angleY -= sensitivity;
  if (key == SDLK_w)angleX += sensitivity;
  if (key == SDLK_s)angleX -= sensitivity;
}

void onKeyUp(vars::Vars&vars){
  ///which key was released
  auto key = vars.getInt32("event.key");
}

void onMouseMotion(vars::Vars&vars){
  ///relative mouse movement
  auto xrel = vars.getInt32("event.mouse.xrel");
  auto yrel = vars.getInt32("event.mouse.yrel");

  ///mouse buttons
  auto left  = vars.getBool("event.mouse.left"  );
  auto mid   = vars.getBool("event.mouse.middle");
  auto right = vars.getBool("event.mouse.right" );

  // Camera variables
  auto sensitivity = vars.getFloat("method.sensitivity");
  auto zoomSpeed = vars.getFloat("method.orbit.zoomSpeed");
  auto& angleX = vars.getFloat("method.orbit.angleX");
  auto& angleY = vars.getFloat("method.orbit.angleY");
  auto& distance = vars.getFloat("method.orbit.distance");

  // Calculate camera movement
  if (mid) {
      angleX += sensitivity * 0.15f * yrel;
      angleY += sensitivity * 0.15f * xrel;

      angleX = glm::clamp(angleX, -glm::half_pi<float>(), glm::half_pi<float>());
  }
  if (right) {
      distance += zoomSpeed * yrel;

      distance = glm::clamp(distance, 0.f, 100.f);
  }
  if (left) {
      // translate model matrix based on camera position and mouse movement
      glm::mat4 inView = glm::inverse(view);
      glm::vec3 objectMotion = glm::vec3(inView * glm::vec4(xrel * 0.01f, -yrel * 0.01f, 0.f, 0.f));
      model = glm::translate(model, objectMotion);
  }

}

// Calculate model matrix from position for light sphere
glm::mat4 calculateModelMatrix(glm::vec3 position, float scale) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));

    return modelMatrix;
}

void onUpdate(vars::Vars&vars){
  ///time delta
  auto dt = vars.getFloat("event.dt");

}



void onInit(vars::Vars&vars){
  //create custom variable into namespace method with default value
  vars.addFloat("method.sensitivity"    ,  0.1f);
  //Camera variables
  vars.addFloat("method.camera.near", 0.10f);
  vars.addFloat("method.camera.far", 100.00f);
  vars.addFloat("method.orbit.angleX", 0.50f);
  vars.addFloat("method.orbit.angleY", 0.50f);
  vars.addFloat("method.orbit.distance", 4.00f);
  vars.addFloat("method.orbit.zoomSpeed", 0.10f);
  vars.add<glm::vec3>("method.orbit.cameraPosition", glm::vec3(1.f));
  vars.addBool("method.resetModelMatrix", false);
  
  // Light Variables
  vars.add<glm::vec3>("method.light.lightPosition", lightPos);
  lightModel = calculateModelMatrix(lightPos, 0.1f);
  addVarsLimits3F(vars, "method.light.lightPosition", -100.f, +100.f, 0.01f);
  vars.add<glm::vec3>("method.light.lightColor", lightColor);
  addVarsLimits3F(vars, "method.light.lightColor", 0.f, +1.f, 0.001f);

  //Wind variables
  vars.add<glm::vec4>("method.wind.Direction", windDirection);
  addVarsLimits4F(vars, "method.wind.Direction", -1.f, +1.f, 0.002f);
  vars.addBool("method.wind.EnableWind", false);
  vars.addUint32("method.wind.Strength", 20);
  addVarsLimitsU(vars, "method.wind.Strength", 0, 40, 1);
  vars.addBool("method.wind.ConstantWind", false);
  vars.addFloat("method.wind.oscilationSpeed", 5.f);
  addVarsLimitsF(vars, "method.wind.oscilationSpeed", 1.f, 20.f, 0.1f);

  //underwater
  vars.addBool("method.underwater.enable", false);


  // Variables for objects
  vars.addUint32("method.object.object", 2);
  addVarsLimitsU(vars, "method.object.object", 0, 2, 1);
  vars.addBool("method.object.viewObject", true);
  vars.add<glm::vec3>("method.object.objectColor", glm::vec3(128.f / 255.f, 56.f / 255.f, 192.f / 255.f));
  addVarsLimits3F(vars, "method.object.objectColor", 0.f, +1.f, 0.001f);
  

  // Variables for hair
  vars.addFloat("method.hair.hairStiffness", 0.08f);
  addVarsLimitsF(vars, "method.hair.hairStiffness", 0.04f, 0.5f, 0.001f);
  vars.add<glm::vec3>("method.hair.hairColor", glm::vec3(90.f / 255.f, 56.f / 255.f, 37.f / 255.f));
  addVarsLimits3F(vars, "method.hair.hairColor", 0.f, +1.f, 0.001f);
  vars.addFloat("method.hair.densityOfTesselation", 800.f);
  addVarsLimitsF(vars, "method.hair.densityOfTesselation", 1.f, 2000.f, 1.f);
  vars.addFloat("method.hair.hairSegmentLength", 0.05f);
  addVarsLimitsF(vars, "method.hair.hairSegmentLength", 0.035f, 0.15f, 0.001f);
  vars.addFloat("method.hair.randomnessIntensity", 0.2f);
  addVarsLimitsF(vars, "method.hair.randomnessIntensity", 0.f, 1.f, 0.01f);
  vars.addUint32("method.hair.constraintItterations", 2);
  addVarsLimitsU(vars, "method.hair.constraintItterations", 1, 5, 1);

  // Basic shader for objects
  auto programStr = readFileToString(std::string(PROJECT_DIR) + "/Shaders/program.glsl");
  program = makeProgram(programStr);
  program->setNonexistingUniformWarning(false);

  // Shader for rendering hair/fur
  auto furShaderStr = readFileToString(std::string(PROJECT_DIR) + "/Shaders/furShader.glsl");
  furShader = makeProgram(furShaderStr);
  furShader->setNonexistingUniformWarning(false);

  // Compute shader for calculating position of hair strands
  auto furComputeStr = readFileToString(std::string(PROJECT_DIR) + "/Shaders/furCompute.glsl");
  furCompute = makeProgram(furComputeStr);
  furCompute->setNonexistingUniformWarning(false);

  // Create objects
  createBox(1.0f, 1.0f, 1.0f, vars);
  createSphere(1.5f, 40, vars);
  createBunny(vars);

  // Create master hairs for all objects
  BoxHairData = createMasterHairs(BoxVertexArray, vars.getInt32("method.box.nverts"));
  SphereHairData = createMasterHairs(SphereVertexArray, vars.getInt32("method.sphere.nverts"));
  BunnyHairData = createMasterHairsforBunny(bunnyVertices, vars.getInt32("method.bunny.nverts"));

  /// add limits to variable
  addVarsLimitsF(vars,"method.sensitivity",-1.f,+1.f,0.01f);

  // Set default starting object
  vao = BunnyVao;
  hairData = BunnyHairData;
  ntris = 3 * vars.getInt32("method.bunny.ntris");
  nverts = vars.getInt32("method.bunny.nverts");
  
  hairDataTextureID_rest = generateTextureFromHairData(hairData, nverts);
  hairDataTextureID_last = generateTextureFromHairData(hairData, nverts);
  hairDataTextureID_current = generateTextureFromHairData(hairData, nverts);
  hairDataTextureID_simulated = generateTextureFromHairData(NULL, nverts);

  // Generate random texture
  randomDataTextureID = createRandomness();

  // Set clear color and depth test
  glClearColor(0.2f,0.2f,0.2f,1.f);
  glEnable(GL_DEPTH_TEST);
}

void reactToObjectChange(vars::Vars& vars) {
    // Seperate object variables and other variables so the textures are not being rewritten every time a light color changes etc.
    if (notChanged(vars, "method.all", __FUNCTION__,
        { "method.object.object", "method.hair.hairSegmentLength",//list of variables
        }))return;

    model = glm::mat4(1.f); // Reset model matrix before calculation

    hairSegmentLength = vars.getFloat("method.hair.hairSegmentLength");

    // Delete last textures
    glDeleteTextures(1, &hairDataTextureID_rest);
    glDeleteTextures(1, &hairDataTextureID_last);
    glDeleteTextures(1, &hairDataTextureID_current);
    glDeleteTextures(1, &hairDataTextureID_simulated);

    // Save values into variables for new object
    if (vars.getUint32("method.object.object") == 0) {
        vao = BoxVao;
        ntris = 3 * vars.getInt32("method.box.ntris");
        nverts = vars.getInt32("method.box.nverts");
        
        delete[] BoxHairData;
        BoxHairData = createMasterHairs(BoxVertexArray, nverts);

        hairData = BoxHairData;
    }
    else if (vars.getUint32("method.object.object") == 1) {
        vao = SphereVao;
        ntris = 3 * vars.getInt32("method.sphere.ntris");
        nverts = vars.getInt32("method.sphere.nverts");

        delete[] SphereHairData;
        SphereHairData = createMasterHairs(SphereVertexArray, nverts);

        hairData = SphereHairData;
    }
    else {
        vao = BunnyVao;
        ntris = 3 * vars.getInt32("method.bunny.ntris");
        nverts = vars.getInt32("method.bunny.nverts");

        delete[] BunnyHairData;
        BunnyHairData = createMasterHairsforBunny(bunnyVertices, nverts);

        hairData = BunnyHairData;
    }

    // Generate new textures for new object or hair length
    hairDataTextureID_rest = generateTextureFromHairData(hairData, nverts);
    hairDataTextureID_last = generateTextureFromHairData(hairData, nverts);
    hairDataTextureID_current = generateTextureFromHairData(hairData, nverts);
    hairDataTextureID_simulated = generateTextureFromHairData(NULL, nverts);
}

void reactToSensitivityChange(vars::Vars&vars){
  // Other uniform variables
  if(notChanged(vars,"method.all",__FUNCTION__,
        {"method.sensitivity", "method.resetModelMatrix", "method.light.lightPosition", "method.light.lightColor", "method.wind.Direction",//list of variables
        }))return;

  if (vars.getBool("method.resetModelMatrix")) {
      model = glm::mat4(1.f);
  }

  lightPos = vars.get<glm::vec3>("method.light.lightPosition")[0];
  lightColor = vars.get<glm::vec3>("method.light.lightColor")[0];

  lightModel = calculateModelMatrix(lightPos, 0.1f);

  windDirection = vars.get<glm::vec4>("method.wind.Direction")[0];
}

void onDraw(vars::Vars&vars){
    // time variables
    chrono::high_resolution_clock::time_point startTime, endTime;
    chrono::duration<double, std::milli> frameDuration;

    // Start time of frame
    startTime = chrono::high_resolution_clock::now();
    
  // Decided to lock to 60 fps, when i run it on high fps monitor it
  // didn't render correctly and this way it's cleaner
  currTime += 0.0167f;
  // React to changes in gui
  reactToSensitivityChange(vars);
  reactToObjectChange(vars);

  // Compute camera matrices
  computeProjectionMatrix(vars);
  computeViewMatrix(vars);

  // Get camera position 
  glm::vec3 cameraPos = calculateCameraPosition(vars);

  // Clear buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Use compute shader
  furCompute->use();

  // Calculate wind magnitude based on sin function for interupted wind
  float windOscilation = vars.getFloat("method.wind.oscilationSpeed");
  windMagnitude += (glm::sin(currTime * windOscilation)) * windOscilation;
  windMagnitude = glm::clamp(windMagnitude, 0.f, 120.f);

  // Bind textures to images in shader
  glBindImageTexture(0, hairDataTextureID_rest, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
  glBindImageTexture(1, hairDataTextureID_last, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
  glBindImageTexture(2, hairDataTextureID_current, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
  glBindImageTexture(3, hairDataTextureID_simulated, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
  
  // Pass uniform variables
  furCompute->setMatrix4fv("model", glm::value_ptr(model));
  furCompute->set1f("hairSegmentLength", hairSegmentLength);
  furCompute->set1f("dt", 0.0167f); // Locked to 60 fps
  furCompute->set1f("hairStiffness", vars.getFloat("method.hair.hairStiffness"));
  furCompute->set1i("constraintIterations", vars.getUint32("method.hair.constraintItterations"));

  // If underwater set lower gravity and disable local constraint to simulate higher draft
  if (vars.getBool("method.underwater.enable")) {
      furCompute->set4f("gravity", 0.f, -0.1f, 0.f, 0.f);
      furCompute->set1f("localConstraint", 0.005f);
      furCompute->set1f("hairDrag", 0.8f);
      glClearColor(142.f / 255.f, 202.f / 255.f, 230.f / 255.f, 1.f); // Change clear color to blue
  }else{
      furCompute->set4f("gravity", 0.f, -20.81f, 0.f, 0.f); // I adjusted the gravity value to better resemble real life
      furCompute->set1f("localConstraint", 0.005f);
      furCompute->set1f("hairDrag", 0.9f);
      glClearColor(0.2f, 0.2f, 0.2f, 1.f);
  }
  
  // If constant wind pass max value always
  if (vars.getBool("method.wind.ConstantWind")) {
      furCompute->set1f("windMagnitude", 120.f * vars.getUint32("method.wind.Strength"));
  }
  else {
      furCompute->set1f("windMagnitude", windMagnitude * vars.getUint32("method.wind.Strength"));
  }
  furCompute->set4fv("windDirection", glm::value_ptr(windDirection));
  furCompute->set1i("windEnabled", vars.getBool("method.wind.EnableWind"));
  // Dispatch compute shader for every master hair
  glDispatchCompute(1, nverts, 1); // every vertex has a master hair so nOfMasterHairs is the same value as number of vertices

  // Change to basic shader
  program->use();

  // set uniform matrices
  program->setMatrix4fv("view", glm::value_ptr(view));
  program->setMatrix4fv("proj", glm::value_ptr(proj));
  program->setMatrix4fv("model", glm::value_ptr(model));

  // set object color
  program->set3fv("color", glm::value_ptr(vars.get<glm::vec3>("method.object.objectColor")[0]));

  // if viewObject is selected render it
  if (vars.getBool("method.object.viewObject")) {
      vao->bind();
      glDrawElements(GL_TRIANGLES, ntris, GL_UNSIGNED_INT, (void*)0);
      vao->unbind();
  }


  // Render light sphere
  SphereVao->bind();

  program->setMatrix4fv("model", glm::value_ptr(lightModel));
  program->set3fv("color", glm::value_ptr(lightColor));

  glDrawElements(GL_TRIANGLES, vars.getInt32("method.sphere.ntris") * 3, GL_UNSIGNED_INT, (void*)0);

  SphereVao->unbind();

  // Wait for compute shader to finish computing
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  
  // Change to shader for rendering hair
  furShader->use();

  glPatchParameteri(GL_PATCH_VERTICES, 3);
  
  // Pass uniform matrices
  furShader->setMatrix4fv("view", glm::value_ptr(view));
  furShader->setMatrix4fv("proj", glm::value_ptr(proj));
  furShader->setMatrix4fv("model", glm::value_ptr(model));

  // Pass uniform values
  furShader->set3fv("lightPosition", glm::value_ptr(lightPos));
  furShader->set3fv("lightColor", glm::value_ptr(lightColor));
  furShader->set3fv("diffuseColor", glm::value_ptr(vars.get<glm::vec3>("method.hair.hairColor")[0]));
  furShader->set3fv("cameraPositionFrag", glm::value_ptr(cameraPos));
  furShader->set3fv("cameraPosition", glm::value_ptr(cameraPos));

  furShader->set1f("nOfHairSegments", (float)nOfHairSegments);
  furShader->set1f("nOfVertices", (float)nverts);
  furShader->set1f("nOfDataVariablesPerMasterHair", (float)nOfDataVariablesForMasterHair);
  furShader->set1f("densityOfTesselation", vars.getFloat("method.hair.densityOfTesselation"));
  furShader->set1f("randomnessIntensity", vars.getFloat("method.hair.randomnessIntensity"));

  // Bind textures to shader
  glBindTextureUnit(0, hairDataTextureID_simulated);
  glBindTextureUnit(1, randomDataTextureID);

  vao->bind();
  glDrawElements(GL_PATCHES, ntris, GL_UNSIGNED_INT, (void*)0);
  vao->unbind();

  // Copy values between textures for next frame
  glCopyImageSubData(hairDataTextureID_current, GL_TEXTURE_2D, 0, 0, 0, 0,
      hairDataTextureID_last, GL_TEXTURE_2D, 0, 0, 0, 0,
      nOfHairSegments, nverts, 1);
  glCopyImageSubData(hairDataTextureID_simulated, GL_TEXTURE_2D, 0, 0, 0, 0,
      hairDataTextureID_current, GL_TEXTURE_2D, 0, 0, 0, 0,
      nOfHairSegments, nverts, 1);

  // get frame duration
  endTime = chrono::high_resolution_clock::now();
  frameDuration = endTime - startTime;

  // while duration is less then 17 miliseconds (60 fps) wait
  while (frameDuration.count() < 17) {
      endTime = chrono::high_resolution_clock::now();
      frameDuration = endTime - startTime;
  }

}

void onResize(vars::Vars&vars){
  /// size of the screen
  auto width  = vars.getUint32("event.resizeX");
  auto height = vars.getUint32("event.resizeY");

  glViewport(0,0,width,height);
}

void onQuit(vars::Vars&vars){
  // free shared pointer values
  vao = nullptr;
  BoxVao = nullptr;
  SphereVao = nullptr;
  BunnyVao = nullptr;
  program = nullptr;
  furShader = nullptr;
  furCompute = nullptr;

  // delete allocated memory for buffers
  delete[] BoxVertexArray;
  delete[] BoxIndexArray;
  delete[] SphereVertexArray;
  delete[] SphereIndexArray;

  delete[] BoxHairData;
  delete[] SphereHairData;
  delete[] BunnyHairData;

  // Delete textures from GPU memory
  glDeleteTextures(1, &hairDataTextureID_rest);
  glDeleteTextures(1, &hairDataTextureID_last);
  glDeleteTextures(1, &hairDataTextureID_current);
  glDeleteTextures(1, &hairDataTextureID_simulated);
  glDeleteTextures(1, &randomDataTextureID);
  
  // free values in method
  vars.erase("method");
}

/// this will register your method into menus and stuff like that...
/// it is using static value initialization and singleton concept...
EntryPoint main = [](){
  /// table of callbacks
  methodManager::Callbacks clbs;
  clbs.onDraw        = onDraw       ;
  clbs.onInit        = onInit       ;
  clbs.onQuit        = onQuit       ;
  clbs.onResize      = onResize     ;
  clbs.onKeyDown     = onKeyDown    ;
  clbs.onKeyUp       = onKeyUp      ;
  clbs.onMouseMotion = onMouseMotion;
  clbs.onUpdate      = onUpdate     ;

  /// register method
  MethodRegister::get().manager.registerMethod("student.project",clbs);
};

}
