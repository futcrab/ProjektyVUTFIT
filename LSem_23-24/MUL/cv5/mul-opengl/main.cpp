//MUL exercise for OpenGL
//Made by Tomas Chlubna and Tomas Milet
//The task is to create a simple video decoder and player.
//The video is stored as YUV sequence of frames.
//Please do not edit any other parts of the code if not necessary, just the highlighted tasks.

//####### TASK 1 #######
//Open the "data" directory with the video
//Download ffmpeg if not installed already: https://ffmpeg.org/download.html
//Use the ffmpeg binary to convert the video with the following command:
//ffmpeg -i stargate.mkv -pix_fmt nv12 stargate.yuv
//The video was converted into a stream of YUV 4:2:0 frames in NV12 format
//To get the resolution and framerate of the input video use:
//ffprobe stargate.mkv
//The finished application should be used as:
//MulGLPlayer inputFile.yuv width height fps
//Additional argument "oldGL" can be used for old OpenGL drivers
//Using the input video:
//MulGLPlayer stargate.yuv 1280 720 23.98

#include <iostream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <SDL.h>
#include <geGL/geGL.h>
#include <geGL/StaticCalls.h>

using namespace ge::gl;

SDL_Window *setupWindow(bool oldGL)
{
    //Using SDL to initialize the window
    auto window = SDL_CreateWindow("MUL OpenGL Player", 50, 50, 1024, 768, SDL_WINDOW_OPENGL);
    int version[2] {4, 6};
    if(oldGL)
    {
        version[0] = 2;
        version[1] = 1;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, version[0]);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, version[1]);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    auto context = SDL_GL_CreateContext(window);
    ge::gl::init();
    return window;
}

GLuint createShader(GLenum type, std::string const &str)
{
    //Shader compilation
    GLuint shader = glCreateShader(type);
    char const *srcs[] = {str.c_str()};
    glShaderSource(shader, 1, srcs, nullptr);
    glCompileShader(shader);

    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled)
    {
        GLint bufferLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufferLen);
        std::vector<GLchar> log(bufferLen);
        glGetShaderInfoLog(shader, bufferLen, &bufferLen, &log[0]);
        std::string logStr(log.begin(), log.end());
        throw std::runtime_error("Cannot compile shaders:\n" + logStr);
    }
    return shader;
}

GLuint setupShaders(bool oldGL)
{
    //Definition of shaders
    std::string version{"#version 460"};
    if(oldGL)
        version = R".(
         #version 300 es
         precision highp float;
    ).";
    std::string vertexShaderSrc = R".(
	layout (location = 0) in vec3 position;
	layout (location = 1) in vec2 inTexCoord;
	out vec2 texCoord;
	void main(){
	gl_Position = vec4(position, 1.0);
	texCoord = inTexCoord;}
  ).";

    std::string fragmentShaderSrc = R".(
	in vec2 texCoord;
	out vec4 color;
	uniform sampler2D frameY;
	uniform sampler2D frameUV;
	void main(){
    int Y = int(255.0f*texture(frameY, vec2(texCoord.x, -texCoord.y)).x);
    ivec2 UV = ivec2(255.0f*texture(frameUV, vec2(texCoord.x, -texCoord.y)).xy);

    //Conversion code source:
    //https://learn.microsoft.com/en-us/windows/win32/medfound/recommended-8-bit-yuv-formats-for-video-rendering#converting-8-bit-yuv-to-rgb888
    int C = Y - 16;
    int D = UV.x - 128;
    int E = UV.y - 128;
    float R = float(clamp(( 298 * C + 409 * E + 128) >> 8, 0, 255))/255.0f;
    float G = float(clamp(( 298 * C - 100 * D - 208 * E + 128) >> 8, 0, 255))/255.0f;
    float B = float(clamp(( 298 * C + 516 * D + 128) >> 8, 0, 255))/255.0f;

	color = vec4(R,G,B,1.0);
	}
	).";

    auto vs = createShader(GL_VERTEX_SHADER, version + vertexShaderSrc);
    auto fs = createShader(GL_FRAGMENT_SHADER, version + fragmentShaderSrc);
    GLuint prg = glCreateProgram();
    glAttachShader(prg, vs);
    glAttachShader(prg, fs);
    glLinkProgram(prg);
    return prg;
}

GLuint setupVertexArray()
{
    //####### TASK 2 #######
    //Create the fullscreen triangle
    //Define three vertices: x,y,z,u,v coordinates (position xyz and texturing uv)
    //No view or model matrices will be used
    //The triangle is defined in NDC - range [-1.0, 1.0]
    //The defined triangle has to cover the whole screen with texturing coordinates:
    //(0,0) at the bottom left corner of the screen
    //(1,1) at the top right corner of the screen
    std::vector<float> vertices
    {
     -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
     3.0f, -1.0f, 0.0f,  2.0f, 0.0f,
     -1.0f, 3.0f, 0.0f,  0.0f, 2.0f
    };
    //####### TASK 2 END #######

    constexpr size_t VERTEX_SIZE{5 * sizeof(GLfloat)};
    constexpr size_t UV_OFFSET{3 * sizeof(GLfloat)};
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glCreateVertexArrays(1, &vao);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (char *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (char *)0 + UV_OFFSET);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return vao;
}

void loadFrame(std::string inputFile, std::vector<GLuint> textures, int width, int height, size_t frameNumber)
{
    //####### TASK 3 #######
    //Open the input file and load the data of the current frame
    //The video format is pretty simple:
    //- the pixels are stored as three 8bit values in YUV colorspace
    //- the chroma (UV values) is subsampled - half of the resolution
    //- one frame consists of full resolution (width X height) array of Y values followed by half resolution U and V values
    //- each pixel has its own intensity Y value but the color values U and V are the same for a block of 2x2 pixels
    // Scheme of the yuv file for video resolution 2x2 and 3 frames, each symbol is one 8bit value:
    // YYYYUVYYYYUVYYYYUV
    //More info about NV12:
    //https://learn.microsoft.com/en-us/windows/win32/medfound/recommended-8-bit-yuv-formats-for-video-rendering#nv12
    //Useful variables:
    //width - width of the input video
    //height - height of the input video
    //frameNumber - number of the current frame
    //inputFile - path to the file

    //The size of the Y and UV data
    size_t sizeY = width * height;
    size_t sizeUV = sizeY / 2;

    //These vectors will contain the Y and UV data
    std::vector<uint8_t> dataY(sizeY);
    std::vector<uint8_t> dataUV(sizeUV);

    //Open the file and fill the vectors
    //...
    std::ifstream video(inputFile, std::ios::binary);
    if (video.is_open()) {
        size_t framePosition = frameNumber * (sizeY + sizeUV);
        video.seekg(framePosition);

        video.read(reinterpret_cast<char*>(dataY.data()), sizeY);
        video.read(reinterpret_cast<char*>(dataUV.data()), sizeUV);
    }

    //####### TASK 3 END #######

    //The loaded data from the file are uploaded in GPU memory as textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, dataY.data());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width / 2, height / 2, 0, GL_RG, GL_UNSIGNED_BYTE, dataUV.data());
}

void draw(SDL_Window *window, GLuint vao, GLuint program)
{
    //Classic OpenGL workflow - clearing framebuffer, binding vertices and drawing
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    SDL_GL_SwapWindow(window);
}

void run(SDL_Window *window, GLuint vao, GLuint program, std::vector<GLuint> textures, std::string filename, float fps, int width, int height)
{
    //Main loop where the drawing function is called - refreshing every frame
    float limitFrameTime{(1.0f / fps) * 1000};
    size_t frameNumber{0};
    bool running = true;
    bool playing = true;
    while(running)
    {
        //Saving the start time at the beginning of the frame computation
        auto start = std::chrono::steady_clock::now();

        //Processing keyboard inputs
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)running = false;
            if(event.type == SDL_KEYDOWN)
            {
                if(event.key.keysym.sym == SDLK_SPACE)
                    playing = !playing;
                if(event.key.keysym.sym == SDLK_ESCAPE)
                    running = false;
            }
        }

        //####### TASK 4 #######
        //Implement fps limiter and stopping of the playback
        //Use the "start" variable that stores beginning time of the frame
        //Measure the frame loading/drawing time
        //Get the difference between expected frame time (from fps) and the real one
        //Delay the frame according to this difference and optionally skip frames in case of lags
        //If spacebar is pressed - variable playing=false - stop the playback
        if (playing) {
            loadFrame(filename, textures, width, height, frameNumber);
            frameNumber++;
            draw(window, vao, program);
            
            auto end = std::chrono::steady_clock::now();
            auto frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            if (frameTime > limitFrameTime) {
                int skipFrames = (int)frameTime / limitFrameTime;
                frameNumber += skipFrames;
            }
            while(frameTime < limitFrameTime){ 
                end = std::chrono::steady_clock::now();
                frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            }
        }
        //####### TASK 4 END #######
    }
}

void setupTexture()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

std::vector<GLuint> createTextures()
{
    constexpr size_t TEXTURE_COUNT{2};
    std::vector<GLuint> textures(TEXTURE_COUNT);
    glGenTextures(TEXTURE_COUNT, textures.data());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    setupTexture();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    setupTexture();
    return textures;
}

int main(int argc, char **argv)
{
    try
    {
        //Processing commandline arguments
        constexpr size_t ARGS_COUNT{5};
        if(argc < ARGS_COUNT)
            throw std::runtime_error("Not enough arguments. Run as: MulGLPlayer inputFile.yuv width height fps\nAdditional argument for mode can be used: \"oldGL\"");
        std::string filename{argv[1]};
        int width = std::stoi(argv[2]);
        int height = std::stoi(argv[3]);
        float fps = std::stof(argv[4]);
        std::string mode{""};
        bool oldGL{false};
        if(argc > ARGS_COUNT)
            mode = argv[5];
        if(mode == "oldGL")
            oldGL = true;

        //Preparing window and static data
        auto window = setupWindow(oldGL);
        auto program = setupShaders(oldGL);
        auto vao = setupVertexArray();
        auto textures = createTextures();

        //Update uniforms - connect samplers with texture units
        glUseProgram(program);
        glUniform1i(glGetUniformLocation(program, "frameY"), 0);
        glUniform1i(glGetUniformLocation(program, "frameUV"), 1);

        //Playback
        run(window, vao, program, textures, filename, fps, width, height);
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
