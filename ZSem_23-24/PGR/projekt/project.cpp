//Radiosity implementation is based on tutorial by Tamas Kormendi
//from this github repo :https://github.com/TamasKormendi/opengl-radiosity-tutorial/blob/master/README.md
//Normal generation is inspired by this: https://computergraphics.stackexchange.com/questions/4031/programmatically-generating-vertex-normals
//File read to string inspired by: https://stackoverflow.com/questions/72510495/how-to-read-a-file-and-write-it-on-a-string-c
//Also used code from PGR exercises

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
#include<BasicCamera/OrbitCamera.h>
#include<BasicCamera/PerspectiveCamera.h>

#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<limits>
#include<cmath>
#include<libs/stb_image/stb_image.h>

#include <iostream>
#include <fstream>
#include <string>

using namespace ge::gl;
using namespace std;

#ifndef CMAKE_ROOT_DIR
#define CMAKE_ROOT_DIR "."
#endif

#define PROJECT_DEP std::string(CMAKE_ROOT_DIR) + "/projekt_dep"

namespace student::project{

	class TextureData {
	public:
		std::vector<uint8_t>data;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t channels = 0;
		TextureData() {}
		TextureData(uint32_t w, uint32_t h, uint32_t c) :width(w), height(h), channels(c) {
			data.resize((size_t)w * h * c, 0);
		}
	};

	TextureData loadTexture(std::string const& fileName) {
		TextureData res;

		int32_t w, h, channels;
		uint8_t* data = stbi_load(fileName.c_str(), &w, &h, &channels, 0);
		if (!data) {
			std::cerr << "Cannot open image file: " << fileName << std::endl;
			return res;
		}
		//std::cerr << "w: " << w << " h: " << h << " c: " << channels << std::endl;
		res.data.resize(w * h * channels);

		for (int32_t y = 0; y < h; ++y)
			for (int32_t x = 0; x < w; ++x)
				for (int32_t c = 0; c < channels; ++c) {
					res.data[(y * w + x) * channels + c] = data[((h - y - 1) * w + x) * channels + c];
				}

		res.channels = channels;
		res.height = h;
		res.width = w;
		stbi_image_free(data);
		return res;
	}

	GLuint createTexture(std::string const& file) {
		auto texData = loadTexture(file);

		GLuint res;
		glCreateTextures(GL_TEXTURE_2D, 1, &res);
		GLenum internalFormat = GL_RGB;
		GLenum format = GL_RGB;
		if (texData.channels == 4) {
			internalFormat = GL_RGBA;
			format = GL_RGBA;
		}
		if (texData.channels == 3) {
			internalFormat = GL_RGB;
			format = GL_RGB;
		}
		if (texData.channels == 2) {
			internalFormat = GL_RG;
			format = GL_RG;
		}
		if (texData.channels == 1) {
			internalFormat = GL_R;
			format = GL_R;
		}

		glTextureImage2DEXT(
			res,//texture
			GL_TEXTURE_2D,//target
			0,//mipmap level
			internalFormat,//gpu format
			texData.width,
			texData.height,
			0,//border
			format,//cpu format
			GL_UNSIGNED_BYTE,//cpu type
			texData.data.data());//pointer to data

		glGenerateTextureMipmap(res);

		glTextureParameteri(res, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(res, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		return res;
	}
	struct ObjectData {
		std::vector<GLfloat> worldspacePosData;
		std::vector<GLfloat> worldspaceNormalData;
		std::vector<GLfloat> idData;
		std::vector<GLfloat> uvData;
		std::vector<int> ShooterIndices;
		float texelArea;
		std::vector<GLfloat> radianceData;
		std::vector<GLfloat> irradianceData;
	};

	ObjectData meshes[6];

	struct TextureVertex {
		float x, y, z;
		float u, v;
		float nx, ny, nz;
		float IDx, IDy, IDz;
	};
	
	std::vector<TextureVertex>objects[5] = {
		{
			{-1.f, 0.f, 2.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{ 2.f, 0.f, 2.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{-1.f, 0.f,-1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},

			{-1.f, 0.f,-1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{ 2.f, 0.f, 2.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{ 2.f, 0.f,-1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		},

		{
			{-1.f, 0.f, 2.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{-1.f, 0.f,-1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{-1.f, 3.f, 2.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
							 	  
			{-1.f, 3.f, 2.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{-1.f, 0.f,-1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{-1.f, 3.f,-1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		},

		{
			{-1.f, 0.f,-1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{ 2.f, 0.f,-1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{-1.f, 3.f,-1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},

			{-1.f, 3.f,-1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{ 2.f, 0.f,-1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{ 2.f, 3.f,-1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		},
		
		{
			{-0.5f, 0.f , 0.5f, 0.f   , 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{ 0.f , 1.f , 0.f , 0.125f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{ 0.5f, 0.f , 0.5f, 0.25f , 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},

			{ 0.5f, 0.f , 0.5f, 0.25f , 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{ 0.f , 1.f , 0.f , 0.375f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{ 0.5f, 0.f ,-0.5f, 0.5f  , 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},

			{ 0.5f, 0.f ,-0.5f, 0.5f  , 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{ 0.f , 1.f , 0.f , 0.625f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{-0.5f, 0.f ,-0.5f, 0.75f , 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},

			{-0.5f, 0.f ,-0.5f, 0.75f , 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{ 0.f , 1.f , 0.f , 0.875f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{-0.5f, 0.f , 0.5f, 1.f  ,  0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		},

		{
			{-0.9f, 0.f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{-0.9f, 2.f,-0.5f, 0.715f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{-0.5f, 0.f,-0.9f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			
			{-0.5f, 0.f,-0.9f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{-0.9f, 2.f,-0.5f, 0.715f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
			{-0.5f, 2.f,-0.9f, 0.715f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		},

	};

	std::vector<TextureVertex>lightCube = {
		{-0.5f,-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.5f,-0.5f, 0.5f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{-0.5f, 0.5f, 0.5f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{-0.5f, 0.5f, 0.5f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.5f,-0.5f, 0.5f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.5f, 0.5f, 0.5f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},

		{ 0.5f,-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.5f,-0.5f,-0.5f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.5f, 0.5f, 0.5f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.5f, 0.5f, 0.5f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.5f,-0.5f,-0.5f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.5f, 0.5f,-0.5f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},

		{ 0.5f,-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{-0.5f,-0.5f,-0.5f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.5f, 0.5f,-0.5f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.5f, 0.5f,-0.5f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{-0.5f,-0.5f,-0.5f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{-0.5f, 0.5f,-0.5f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},

		{-0.5f,-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{-0.5f,-0.5f, 0.5f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{-0.5f, 0.5f,-0.5f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{-0.5f, 0.5f,-0.5f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{-0.5f,-0.5f, 0.5f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{-0.5f, 0.5f, 0.5f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},

		{-0.5f,-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.5f,-0.5f, 0.5f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{-0.5f,-0.5f,-0.5f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{-0.5f,-0.5f,-0.5f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.5f,-0.5f, 0.5f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.5f,-0.5f,-0.5f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},

		{-0.5f, 0.5f, 0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.5f, 0.5f, 0.5f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{-0.5f, 0.5f,-0.5f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{-0.5f, 0.5f,-0.5f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.5f, 0.5f, 0.5f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.5f, 0.5f,-0.5f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
	};

	std::vector<GLfloat> screenAlignedQuadVertices = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	GLuint vao[5];
	GLuint vbo[5];
	GLuint tex[5];

	GLuint lightCubeAO;
	GLuint lightCubeBO;
	GLuint lightCubeTex;

	bool doRadiosityIteration = true;

	unsigned int preprocessDone = 0;

	//Need to select a mesh when the iteration first starts
	bool meshSelectionNeeded = true;
	unsigned int shooterIndex = 0;
	unsigned int shooterMesh = 0;

glm::vec3 calculateTriangleID(int TriangleID) {
	glm::vec3 tID;
	int redValue = TriangleID % 256;
	int greenRemainingValue = (int)TriangleID / 256;
	int greenValue = greenRemainingValue % 256;
	int blueRemainingValue = (int)greenRemainingValue / 256;
	int blueValue = blueRemainingValue % 256;

	tID.x =(float) redValue / 255.f;
	tID.y = (float) greenValue / 255.f;
	tID.z = (float) blueValue /255.f;

	return tID;
}

void addNormals(TextureVertex* V1, TextureVertex* V2, TextureVertex* V3, int TriangleID) {

	glm::vec3 A(V1->x - V2->x, V1->y - V2->y, V1->z - V2->z);
	glm::vec3 B(V1->x - V3->x, V1->y - V3->y, V1->z - V3->z);
	auto normal = glm::normalize(glm::cross(A,B));
	auto tID = calculateTriangleID(TriangleID);

	V1->nx = normal.x;
	V1->ny = normal.y;
	V1->nz = normal.z;
	V1->IDx = tID.x;
	V1->IDy = tID.y;
	V1->IDz = tID.z;

	V2->nx = normal.x;
	V2->ny = normal.y;
	V2->nz = normal.z;
	V2->IDx = tID.x;
	V2->IDy = tID.y;
	V2->IDz = tID.z;

	V3->nx = normal.x;
	V3->ny = normal.y;
	V3->nz = normal.z;
	V3->IDx = tID.x;
	V3->IDy = tID.y;
	V3->IDz = tID.z;
}

template<typename T>

GLuint createbuffer(std::vector<T> const data) {
	GLuint vbo;
	glCreateBuffers(1, &vbo);
	glNamedBufferData(vbo, data.size() * sizeof(decltype(data)::value_type), data.data(), GL_DYNAMIC_DRAW);
	return vbo;
}

void addAttrib(GLuint vao, GLint a, GLint n, GLenum t, GLsizei o, GLsizei s, GLuint vbo) {
	glVertexArrayAttribBinding(vao, a, a);
	glEnableVertexArrayAttrib(vao, a);
	glVertexArrayAttribFormat(vao, a, n, t, GL_FALSE, 0);
	glVertexArrayVertexBuffer(vao, a, vbo, o, s);
}

void onKeyDown(vars::Vars&vars){
  ///which key was pressed
  auto key = vars.getInt32("event.key");
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

  //info on window
  auto width = vars.getInt32("event.resizeX");
  auto height = vars.getInt32("event.resizeY");

  auto sensitivity = vars.getFloat("method.sensitivity");
  auto zoomSpeed = vars.getFloat("method.orbit.zoomSpeed");

  auto& view = *vars.get<basicCamera::OrbitCamera>("method.view");

  if (vars.getBool("event.mouse.left")) {
	  view.addXAngle(sensitivity * yrel);
	  view.addYAngle(sensitivity * xrel);
  }
  if (vars.getBool("event.mouse.middle")) {
	  view.addXPosition(+view.getDistance() * xrel /
		  float(width) * 2.f);
	  view.addYPosition(-view.getDistance() * yrel /
		  float(height) * 2.f);
  }
  if (vars.getBool("event.mouse.right")) {
	  view.addDistance(zoomSpeed * yrel);
  }

}

void onUpdate(vars::Vars&vars){
  ///time delta
  auto dt = vars.getFloat("event.dt");

}

void setUpCamera(vars::Vars& vars) {
	vars.addFloat("method.sensitivity", 0.01f);
	vars.addFloat("method.near", 0.10f);
	vars.addFloat("method.far", 1000.00f);
	vars.addFloat("method.orbit.zoomSpeed", 0.10f);
	vars.reCreate<basicCamera::OrbitCamera		>("method.view");
	vars.reCreate<basicCamera::PerspectiveCamera>("method.proj");
}

void computeProjectionMatrix(vars::Vars& vars) {
	auto width = vars.getUint32("event.resizeX");
	auto height = vars.getUint32("event.resizeY");
	auto near = vars.getFloat("method.near");
	auto far = vars.getFloat("method.far");

	float aspect = static_cast<float>(width) / static_cast<float>(height);
	auto pr = vars.get<basicCamera::PerspectiveCamera>("method.proj");
	pr->setFar(far);
	pr->setNear(near);
	pr->setAspect(aspect);
}

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

void CreateTex_empty(GLuint* tex, GLsizei mapSize, GLenum attachment) {
	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mapSize, mapSize, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, *tex, 0);
};

void GenRenderBuffers(GLuint* buff, GLsizei mapSize, GLenum attachment) {
	glGenRenderbuffers(1, buff);
	glBindRenderbuffer(GL_RENDERBUFFER, *buff);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mapSize, mapSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, *buff);
}

void preprocess(vars::Vars& vars) {
	auto mapSize = (GLsizei)pow(2, vars.getUint32("method.radiosity.RadiosityTexture_size"));

	GLuint preprocessFramebuffer, worldspacePosData, worldspaceNormalData, idData, uvData;

	glGenFramebuffers(1, &preprocessFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, preprocessFramebuffer);

	CreateTex_empty(&worldspacePosData, mapSize, GL_COLOR_ATTACHMENT0);
	CreateTex_empty(&worldspaceNormalData, mapSize, GL_COLOR_ATTACHMENT1);
	CreateTex_empty(&idData, mapSize, GL_COLOR_ATTACHMENT2);
	CreateTex_empty(&uvData, mapSize, GL_COLOR_ATTACHMENT3);

	//Depth buffer
	GLuint depth;
	GenRenderBuffers(&depth, mapSize, GL_DEPTH_ATTACHMENT);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer isn't complete" << std::endl;
	}

	GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

	glDrawBuffers(4, attachments);
	glViewport(0, 0, mapSize, mapSize);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	auto prg = vars.get<ge::gl::Program>("method.preprocess_s");

	for (int i = 0; i < 6; i++) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		prg->use();

		//Create readback buffers for all of the texture data
		std::vector<GLfloat> worldspacePositionDataBuffer(mapSize * mapSize * 3);
		std::vector<GLfloat> normalVectorDataBuffer(mapSize * mapSize * 3);

		std::vector<GLfloat> idDataBuffer(mapSize * mapSize * 3);
		std::vector<GLfloat> uvDataBuffer(mapSize * mapSize * 3);

		if (i == 5) {
			glm::mat4 lampModel = glm::mat4(1);
			glm::vec3 lightPos = *vars.get<glm::vec3>("method.light.LightPos");
			float scale = vars.getFloat("method.light.LightCube_size");

			lampModel = glm::translate(lampModel, lightPos);
			lampModel = glm::scale(lampModel, glm::vec3(scale));

			prg->setMatrix4fv("model", glm::value_ptr(lampModel))->use();

			glBindVertexArray(lightCubeAO);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)lightCube.size());

			meshes[i].irradianceData = std::vector<GLfloat>(mapSize * mapSize * 3, 1.0f);
			meshes[i].radianceData = std::vector<GLfloat>(mapSize * mapSize * 3, 6.0f);
		}
		else {
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			prg->setMatrix4fv("model", glm::value_ptr(modelMatrix))->use();

			glBindVertexArray(vao[i]);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)objects[i].size());

			meshes[i].irradianceData = std::vector<GLfloat>(mapSize * mapSize * 3, 0.0f);
			meshes[i].radianceData = std::vector<GLfloat>(mapSize * mapSize * 3, 0.0f);
		}

		//Read the data back to the CPU
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glReadPixels(0, 0, mapSize, mapSize, GL_RGB, GL_FLOAT, &worldspacePositionDataBuffer[0]);
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glReadPixels(0, 0, mapSize, mapSize, GL_RGB, GL_FLOAT, &normalVectorDataBuffer[0]);
		glReadBuffer(GL_COLOR_ATTACHMENT2);
		glReadPixels(0, 0, mapSize, mapSize, GL_RGB, GL_FLOAT, &idDataBuffer[0]);
		glReadBuffer(GL_COLOR_ATTACHMENT3);
		glReadPixels(0, 0, mapSize, mapSize, GL_RGB, GL_FLOAT, &uvDataBuffer[0]);

		meshes[i].worldspacePosData = worldspacePositionDataBuffer;
		meshes[i].worldspaceNormalData = normalVectorDataBuffer;
		meshes[i].idData = idDataBuffer;
		meshes[i].uvData = uvDataBuffer;
		

		for (unsigned int j = 0; j < meshes[i].idData.size(); j += 3) {
			float redIDValue = meshes[i].idData[j];
			float greenIDValue = meshes[i].idData[j + 1];
			float blueIDValue = meshes[i].idData[j + 2];

			float idSum = redIDValue + greenIDValue + blueIDValue;

			if (idSum > 0) {
				meshes[i].ShooterIndices.push_back(j);
			}
		}
		meshes[i].texelArea = 1.0;
	}
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glViewport(0, 0, vars.getUint32("event.resizeX"), vars.getUint32("event.resizeY"));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteFramebuffers(1, &preprocessFramebuffer);
	glDeleteTextures(1, &worldspacePosData);
	glDeleteTextures(1, &worldspaceNormalData);
	glDeleteTextures(1, &idData);
	glDeleteTextures(1, &uvData);
};

unsigned int selectShooterMesh(vars::Vars& vars) {
	GLuint shooterSelectionFramebuffer, meshIDTexture, depth, textureLoc;

	glGenFramebuffers(1, &shooterSelectionFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shooterSelectionFramebuffer);

	CreateTex_empty(&meshIDTexture, 1, GL_COLOR_ATTACHMENT0);
	GenRenderBuffers(&depth, 1, GL_DEPTH_ATTACHMENT);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer isn't complete" << std::endl;
	}

	unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);
	glViewport(0, 0, 1, 1);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto prg = vars.get<ge::gl::Program>("method.shooterMeshSelection_s");
	auto vao = vars.get<ge::gl::VertexArray>("method.screenAlignedVAO");

	prg->use();
	vao->bind();
	for (int i = 0; i < 6; i++) {
		unsigned int meshID = i;

		//Calculate RGB ID based on meshID
		float redValue = meshID % 100;
		int greenRemainingValue = (int)meshID / 100;
		float greenValue = greenRemainingValue % 100;
		int blueRemainingValue = (int)greenRemainingValue / 100;
		float blueValue = blueRemainingValue % 100;

		redValue = redValue / 100.f;
		greenValue = greenValue / 100.f;
		blueValue = blueValue / 100.f;

		glm::vec3 meshIDVector = glm::vec3(redValue, greenValue, blueValue);

		prg->set3fv("meshID", glm::value_ptr(meshIDVector));

		auto mapSize = (GLsizei)pow(2, vars.getUint32("method.radiosity.RadiosityTexture_size"));

		GLuint mipmappedRadianceTexture;

		glGenTextures(1, &mipmappedRadianceTexture);
		glBindTexture(GL_TEXTURE_2D, mipmappedRadianceTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mapSize, mapSize, 0, GL_RGB, GL_FLOAT, &meshes[i].radianceData[0]);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glActiveTexture(GL_TEXTURE0);
			
		textureLoc = prg->getAttribLocation("mipmappedRadianceTexture");
		glUniform1i(textureLoc, 0);
		glBindTexture(GL_TEXTURE_2D, mipmappedRadianceTexture);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDeleteTextures(1, &mipmappedRadianceTexture);
	}
	std::vector<GLfloat> shooterMeshID(1 * 1 * 3);

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, 1, 1, GL_RGB, GL_FLOAT, &shooterMeshID[0]);

	unsigned int chosenShooterMeshID = 0;

	chosenShooterMeshID += (int)std::round(100 * 100 * 100 * shooterMeshID[2]);
	chosenShooterMeshID += (int)std::round(100 * 100 * shooterMeshID[1]);
	chosenShooterMeshID += (int)std::round(100 * shooterMeshID[0]);

	glViewport(0, 0, vars.getUint32("event.resizeX"), vars.getUint32("event.resizeY"));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glDeleteFramebuffers(1, &shooterSelectionFramebuffer);
	glDeleteRenderbuffers(1, &depth);
	glDeleteTextures(1, &meshIDTexture);

	return chosenShooterMeshID;
}

void selectMeshBasedShooter(vars::Vars& vars, glm::vec3& shooterRadiance, glm::vec3& shooterWorldspacePos, 
		glm::vec3& shooterWorldspaceNormal, glm::vec3& shooterUV, unsigned int& shooterMeshIndex) {
	
	unsigned int texelIndex = meshes[shooterMeshIndex].ShooterIndices[shooterIndex];

	shooterRadiance[0] = meshes[shooterMeshIndex].radianceData[texelIndex] / meshes[shooterMeshIndex].ShooterIndices.size();
	shooterRadiance[1] = meshes[shooterMeshIndex].radianceData[texelIndex + 1] / meshes[shooterMeshIndex].ShooterIndices.size();
	shooterRadiance[2] = meshes[shooterMeshIndex].radianceData[texelIndex + 2] / meshes[shooterMeshIndex].ShooterIndices.size();

	shooterWorldspacePos[0] = meshes[shooterMeshIndex].worldspacePosData[texelIndex];
	shooterWorldspacePos[1] = meshes[shooterMeshIndex].worldspacePosData[texelIndex + 1];
	shooterWorldspacePos[2] = meshes[shooterMeshIndex].worldspacePosData[texelIndex + 2];

	shooterWorldspaceNormal[0] = meshes[shooterMeshIndex].worldspaceNormalData[texelIndex];
	shooterWorldspaceNormal[1] = meshes[shooterMeshIndex].worldspaceNormalData[texelIndex + 1];
	shooterWorldspaceNormal[2] = meshes[shooterMeshIndex].worldspaceNormalData[texelIndex + 2];

	
	shooterUV[0] = meshes[shooterMeshIndex].uvData[texelIndex];
	shooterUV[1] = meshes[shooterMeshIndex].uvData[texelIndex + 1];
	shooterUV[2] = meshes[shooterMeshIndex].uvData[texelIndex + 2];

	meshes[shooterMeshIndex].radianceData[texelIndex] = 0.f;
	meshes[shooterMeshIndex].radianceData[texelIndex+1] = 0.f;
	meshes[shooterMeshIndex].radianceData[texelIndex+2] = 0.f;

	if (shooterIndex >= meshes[shooterMeshIndex].ShooterIndices.size() - 1) {
		meshSelectionNeeded = true;
		shooterIndex = 0;
	}
	else {
		shooterIndex++;
	}
}

unsigned int createHemicubeDepthMapTexture(const unsigned int resolution) {
	float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	unsigned int depthMapTexture;

	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//This part is needed to avoid light bleeding by oversampling (so sampling outside the depth texture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	return depthMapTexture;
}

std::vector<unsigned int> createHemicubeTextures(
	vars::Vars& vars,
	std::vector<glm::mat4>& viewMatrices,
	unsigned int& resolution,
	glm::vec3& shooterWorldspacePos,
	glm::vec3& shooterWorldspaceNormal) {

	float borderColor[] = { 0.f, 0.f, 0.f, 1.f };

	glm::vec3 worldUp = glm::vec3(0.f, 1.f, 0.f);

	glm::vec3 normalisedShooterNormal = glm::normalize(shooterWorldspaceNormal);

	if (normalisedShooterNormal.x == 0.0f && normalisedShooterNormal.y == 1.0f && normalisedShooterNormal.z == 0.0f) {
		worldUp = glm::vec3(0.0f, 0.0f, -1.0f);
	}
	else if (normalisedShooterNormal.x == 0.0f && normalisedShooterNormal.y == -1.0f && normalisedShooterNormal.z == 0.0f) {
		worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
	}

	glm::vec3 hemicubeRight = glm::normalize(glm::cross(shooterWorldspaceNormal, worldUp));
	glm::vec3 hemicubeUp = glm::normalize(glm::cross(hemicubeRight, shooterWorldspaceNormal));

	glm::mat4 frontShooterView = glm::lookAt(shooterWorldspacePos, shooterWorldspacePos + shooterWorldspaceNormal, hemicubeUp);

	glm::mat4 leftShooterView = glm::lookAt(shooterWorldspacePos, shooterWorldspacePos + (-hemicubeRight), hemicubeUp);
	glm::mat4 rightShooterView = glm::lookAt(shooterWorldspacePos, shooterWorldspacePos + hemicubeRight, hemicubeUp);

	glm::mat4 upShooterView = glm::lookAt(shooterWorldspacePos, shooterWorldspacePos + hemicubeUp, -normalisedShooterNormal);
	glm::mat4 downShooterView = glm::lookAt(shooterWorldspacePos, shooterWorldspacePos + (-hemicubeUp), normalisedShooterNormal);
	
	viewMatrices.push_back(frontShooterView);

	viewMatrices.push_back(leftShooterView);
	viewMatrices.push_back(rightShooterView);

	viewMatrices.push_back(upShooterView);
	viewMatrices.push_back(downShooterView);
	
	unsigned int hemicubeFramebuffer;
	glGenFramebuffers(1, &hemicubeFramebuffer);

	unsigned int frontDepthMap;
	glGenTextures(1, &frontDepthMap);
	glBindTexture(GL_TEXTURE_2D, frontDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//This part is needed to avoid light bleeding by oversampling (so sampling outside the depth texture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, hemicubeFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frontDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glViewport(0, 0, resolution, resolution);

	glClear(GL_DEPTH_BUFFER_BIT);

	auto prg = vars.get<ge::gl::Program>("method.HemicubeVisTex_s");

	for (int i = 0; i < 6; i++) {
		prg
			->setMatrix4fv("view", glm::value_ptr(frontShooterView))
			->use();
		if (i == 5) {
			glm::mat4 lampModel = glm::mat4(1);
			glm::vec3 lightPos = *vars.get<glm::vec3>("method.light.LightPos");
			float scale = vars.getFloat("method.light.LightCube_size");

			lampModel = glm::translate(lampModel, lightPos);
			lampModel = glm::scale(lampModel, glm::vec3(scale));

			prg->setMatrix4fv("model", glm::value_ptr(lampModel));

			glBindVertexArray(lightCubeAO);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)lightCube.size());
		}
		else {
			prg->setMatrix4fv("model", glm::value_ptr(glm::mat4(1)));

			glBindVertexArray(vao[i]);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)objects[i].size());

		}
	}

	unsigned int leftDepthMap;
	glGenTextures(1, &leftDepthMap);
	glBindTexture(GL_TEXTURE_2D, leftDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//This part is needed to avoid light bleeding by oversampling (so sampling outside the depth texture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, hemicubeFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, leftDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glViewport(0, 0, resolution, resolution);

	glClear(GL_DEPTH_BUFFER_BIT);

	prg = vars.get<ge::gl::Program>("method.HemicubeVisTex_s");

	for (int i = 0; i < 6; i++) {
		prg
			->setMatrix4fv("view", glm::value_ptr(frontShooterView))
			->use();
		if (i == 5) {
			glm::mat4 lampModel = glm::mat4(1);
			glm::vec3 lightPos = *vars.get<glm::vec3>("method.light.LightPos");
			float scale = vars.getFloat("method.light.LightCube_size");

			lampModel = glm::translate(lampModel, lightPos);
			lampModel = glm::scale(lampModel, glm::vec3(scale));

			prg->setMatrix4fv("model", glm::value_ptr(lampModel));

			glBindVertexArray(lightCubeAO);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)lightCube.size());
		}
		else {
			prg->setMatrix4fv("model", glm::value_ptr(glm::mat4(1)));

			glBindVertexArray(vao[i]);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)objects[i].size());

		}
	}

	unsigned int rightDepthMap;
	glGenTextures(1, &rightDepthMap);
	glBindTexture(GL_TEXTURE_2D, rightDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//This part is needed to avoid light bleeding by oversampling (so sampling outside the depth texture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, hemicubeFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rightDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glViewport(0, 0, resolution, resolution);

	glClear(GL_DEPTH_BUFFER_BIT);

	prg = vars.get<ge::gl::Program>("method.HemicubeVisTex_s");

	for (int i = 0; i < 6; i++) {
		prg
			->setMatrix4fv("view", glm::value_ptr(rightShooterView))
			->use();
		if (i == 5) {
			glm::mat4 lampModel = glm::mat4(1);
			glm::vec3 lightPos = *vars.get<glm::vec3>("method.light.LightPos");
			float scale = vars.getFloat("method.light.LightCube_size");

			lampModel = glm::translate(lampModel, lightPos);
			lampModel = glm::scale(lampModel, glm::vec3(scale));

			prg->setMatrix4fv("model", glm::value_ptr(lampModel));

			glBindVertexArray(lightCubeAO);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)lightCube.size());
		}
		else {
			prg->setMatrix4fv("model", glm::value_ptr(glm::mat4(1)));

			glBindVertexArray(vao[i]);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)objects[i].size());

		}
	}

	unsigned int upDepthMap;
	glGenTextures(1, &upDepthMap);
	glBindTexture(GL_TEXTURE_2D, upDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//This part is needed to avoid light bleeding by oversampling (so sampling outside the depth texture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, hemicubeFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, upDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glViewport(0, 0, resolution, resolution);

	glClear(GL_DEPTH_BUFFER_BIT);

	prg = vars.get<ge::gl::Program>("method.HemicubeVisTex_s");

	for (int i = 0; i < 6; i++) {
		prg
			->setMatrix4fv("view", glm::value_ptr(upShooterView))
			->use();
		if (i == 5) {
			glm::mat4 lampModel = glm::mat4(1);
			glm::vec3 lightPos = *vars.get<glm::vec3>("method.light.LightPos");
			float scale = vars.getFloat("method.light.LightCube_size");

			lampModel = glm::translate(lampModel, lightPos);
			lampModel = glm::scale(lampModel, glm::vec3(scale));

			prg->setMatrix4fv("model", glm::value_ptr(lampModel));

			glBindVertexArray(lightCubeAO);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)lightCube.size());
		}
		else {
			prg->setMatrix4fv("model", glm::value_ptr(glm::mat4(1)));

			glBindVertexArray(vao[i]);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)objects[i].size());

		}
	}

	unsigned int downDepthMap;
	glGenTextures(1, &downDepthMap);
	glBindTexture(GL_TEXTURE_2D, downDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//This part is needed to avoid light bleeding by oversampling (so sampling outside the depth texture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, hemicubeFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, downDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glViewport(0, 0, resolution, resolution);

	glClear(GL_DEPTH_BUFFER_BIT);

	prg = vars.get<ge::gl::Program>("method.HemicubeVisTex_s");

	for (int i = 0; i < 6; i++) {
		prg
			->setMatrix4fv("view", glm::value_ptr(downShooterView))
			->use();
		if (i == 5) {
			glm::mat4 lampModel = glm::mat4(1);
			glm::vec3 lightPos = *vars.get<glm::vec3>("method.light.LightPos");
			float scale = vars.getFloat("method.light.LightCube_size");

			lampModel = glm::translate(lampModel, lightPos);
			lampModel = glm::scale(lampModel, glm::vec3(scale));

			prg->setMatrix4fv("model", glm::value_ptr(lampModel));

			glBindVertexArray(lightCubeAO);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)lightCube.size());
		}
		else {
			prg->setMatrix4fv("model", glm::value_ptr(glm::mat4(1)));

			glBindVertexArray(vao[i]);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)objects[i].size());

		}
	}

	glViewport(0, 0, vars.getUint32("event.resizeX"), vars.getUint32("event.resizeY"));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<unsigned int> depthTextures;
	
	depthTextures.push_back(frontDepthMap);

	depthTextures.push_back(leftDepthMap);
	depthTextures.push_back(rightDepthMap);

	depthTextures.push_back(upDepthMap);
	depthTextures.push_back(downDepthMap);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glDeleteFramebuffers(1, &hemicubeFramebuffer);

	return depthTextures;
}

void updateLighmaps(vars::Vars& vars, std::vector<glm::mat4>& viewMatrices, std::vector<unsigned int>& visibilityTextures) {
	GLuint lightmapFramebuffer, newIrradianceTexture, newRadianceTexture, depth, textureLoc;

	glGenFramebuffers(1, &lightmapFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, lightmapFramebuffer);

	auto mapSize = (GLsizei)pow(2, vars.getUint32("method.radiosity.RadiosityTexture_size"));

	CreateTex_empty(&newIrradianceTexture, mapSize, GL_COLOR_ATTACHMENT0);
	CreateTex_empty(&newRadianceTexture, mapSize, GL_COLOR_ATTACHMENT1);

	GenRenderBuffers(&depth, mapSize, GL_DEPTH_ATTACHMENT);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer isn't complete" << std::endl;
	}

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	glDrawBuffers(2, attachments);
	glViewport(0, 0, mapSize, mapSize);
	glClearColor(0.f, 0.f, 0.f, 1.f);

	auto prg = vars.get<ge::gl::Program>("method.LightmapUpdate_s");
	prg
		->setMatrix4fv("view", glm::value_ptr(viewMatrices[0]))
		->setMatrix4fv("leftView", glm::value_ptr(viewMatrices[1]))
		->setMatrix4fv("rightView", glm::value_ptr(viewMatrices[2]))
		->setMatrix4fv("upView", glm::value_ptr(viewMatrices[3]))
		->setMatrix4fv("downView", glm::value_ptr(viewMatrices[4]))
		->set1i("attenuationType", 0)
		->use();

	glActiveTexture(GL_TEXTURE2);
	textureLoc = prg->getAttribLocation("visibilityTexture");
	glUniform1i(textureLoc, 2);
	glBindTexture(GL_TEXTURE_2D, visibilityTextures[0]);

	glActiveTexture(GL_TEXTURE3);
	textureLoc = prg->getAttribLocation("leftVisibilityTexture");
	glUniform1i(textureLoc, 3);
	glBindTexture(GL_TEXTURE_2D, visibilityTextures[1]);

	glActiveTexture(GL_TEXTURE4);
	textureLoc = prg->getAttribLocation("rightVisibilityTexture");
	glUniform1i(textureLoc, 4);
	glBindTexture(GL_TEXTURE_2D, visibilityTextures[2]);

	glActiveTexture(GL_TEXTURE5);
	textureLoc = prg->getAttribLocation("upVisibilityTexture");
	glUniform1i(textureLoc, 5);
	glBindTexture(GL_TEXTURE_2D, visibilityTextures[3]);

	glActiveTexture(GL_TEXTURE6);
	textureLoc = prg->getAttribLocation("downVisibilityTexture");
	glUniform1i(textureLoc, 6);
	glBindTexture(GL_TEXTURE_2D, visibilityTextures[4]);

	glActiveTexture(GL_TEXTURE0);

	for (int i = 0; i < 6; i++){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::vector<GLfloat> newIrradianceDataBuffer(mapSize * mapSize * 3);
		std::vector<GLfloat> newRadianceDataBuffer(mapSize * mapSize * 3);

		prg->set1f("texelArea", meshes[i].texelArea);

		//Create textures from the old irradiance and radiance data
		GLuint irradianceID;
		glGenTextures(1, &irradianceID);

		glBindTexture(GL_TEXTURE_2D, irradianceID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mapSize, mapSize, 0, GL_RGB, GL_FLOAT, &meshes[i].irradianceData[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		GLuint radianceID;
		glGenTextures(1, &radianceID);

		glBindTexture(GL_TEXTURE_2D, radianceID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mapSize, mapSize, 0, GL_RGB, GL_FLOAT, &meshes[i].radianceData[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		//Bind them
		glActiveTexture(GL_TEXTURE0);
		textureLoc = prg->getAttribLocation("irradianceTexture");
		glUniform1i(textureLoc, 0);
		glBindTexture(GL_TEXTURE_2D, irradianceID);

		glActiveTexture(GL_TEXTURE1);
		textureLoc = prg->getAttribLocation("radianceTexture");
		glUniform1i(textureLoc, 1);
		glBindTexture(GL_TEXTURE_2D, radianceID);

		if (i == 5) {
			glm::mat4 lampModel = glm::mat4(1);
			glm::vec3 lightPos = *vars.get<glm::vec3>("method.light.LightPos");
			float scale = vars.getFloat("method.light.LightCube_size");

			lampModel = glm::translate(lampModel, lightPos);
			lampModel = glm::scale(lampModel, glm::vec3(scale));

			prg->setMatrix4fv("model", glm::value_ptr(lampModel));
			prg->set1i("isLamp", 1);

			glBindVertexArray(lightCubeAO);
			glBindTextureUnit(7, lightCubeTex);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)lightCube.size());
		}
		else
		{
			prg->setMatrix4fv("model", glm::value_ptr(glm::mat4(1)));
			prg->set1i("isLamp", 0);

			glBindVertexArray(vao[i]);
			glBindTextureUnit(7, tex[i]);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)objects[i].size());
		}

		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glReadPixels(0, 0, mapSize, mapSize, GL_RGB, GL_FLOAT, &newIrradianceDataBuffer[0]);
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glReadPixels(0, 0, mapSize, mapSize, GL_RGB, GL_FLOAT, &newRadianceDataBuffer[0]);

		meshes[i].irradianceData = newIrradianceDataBuffer;
		meshes[i].radianceData = newRadianceDataBuffer;

		glDeleteTextures(1, &irradianceID);
		glDeleteTextures(1, &radianceID);
	}
	glDeleteTextures(1, &newIrradianceTexture);
	glDeleteTextures(1, &newRadianceTexture);
	glDeleteRenderbuffers(1, &depth);
	glDeleteFramebuffers(1, &lightmapFramebuffer);

	glViewport(0, 0, vars.getUint32("event.resizeX"), vars.getUint32("event.resizeY"));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void onInit(vars::Vars&vars){
	setUpCamera(vars);
  //create custom variable into namespace method with default value
  vars.addFloat("method.light.LightCube_size"    ,  0.1f);
  vars.add<glm::vec3>("method.light.LightPos", glm::vec3(1.f));
  vars.addUint32("method.radiosity.RadiosityTexture_size", 5);
  vars.addBool("method.radiosity.textureFiltering", false);
  vars.addBool("method.radiosity.addAmbient", true);

  /// add limits to variable
  addVarsLimitsF(vars,"method.light.LightCube_size",0.f, std::numeric_limits<float>::max(), 0.01f);
  addVarsLimits3F(vars, "method.light.LightPos", -50.f, 50.f, 0.05f);
  addVarsLimitsU(vars, "method.radiosity.RadiosityTexture_size", 2, 9, 1);


  auto vertexPrg = readFileToString(std::string(PROJECT_DEP) + "/shaders/vertex.glsl");
  auto fragmentPrg = readFileToString(std::string(PROJECT_DEP) + "/shaders/fragment.glsl");

  vars.reCreate<ge::gl::Program>("method.prg",
	  std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, "#version 460\n#define   VERTEX_SHADER\n" + vertexPrg),
	  std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, "#version 460\n#define FRAGMENT_SHADER\n" + fragmentPrg)
  );

  vertexPrg = readFileToString(std::string(PROJECT_DEP) + "/shaders/Preprocess.vs");
  fragmentPrg = readFileToString(std::string(PROJECT_DEP) + "/shaders/Preprocess.fs");

  vars.reCreate<ge::gl::Program>("method.preprocess_s",
	  std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, "#version 460\n#define   VERTEX_SHADER\n" + vertexPrg),
	  std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, "#version 460\n#define FRAGMENT_SHADER\n" + fragmentPrg)
  );

  vertexPrg = readFileToString(std::string(PROJECT_DEP) + "/shaders/shooterMeshSelection.vs");
  fragmentPrg = readFileToString(std::string(PROJECT_DEP) + "/shaders/shooterMeshSelection.fs");

  vars.reCreate<ge::gl::Program>("method.shooterMeshSelection_s",
	  std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, "#version 460\n#define   VERTEX_SHADER\n" + vertexPrg),
	  std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, "#version 460\n#define FRAGMENT_SHADER\n" + fragmentPrg)
  );

  vertexPrg = readFileToString(std::string(PROJECT_DEP) + "/shaders/HemicubeVisTex.vs");
  fragmentPrg = readFileToString(std::string(PROJECT_DEP) + "/shaders/HemicubeVisTex.fs");

  vars.reCreate<ge::gl::Program>("method.HemicubeVisTex_s",
	  std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, "#version 460\n#define   VERTEX_SHADER\n" + vertexPrg),
	  std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, "#version 460\n#define FRAGMENT_SHADER\n" + fragmentPrg)
  );

  vertexPrg = readFileToString(std::string(PROJECT_DEP) + "/shaders/LightmapUpdate.vs");
  fragmentPrg = readFileToString(std::string(PROJECT_DEP) + "/shaders/LightmapUpdate.fs");

  vars.reCreate<ge::gl::Program>("method.LightmapUpdate_s",
	  std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, "#version 460\n#define   VERTEX_SHADER\n" + vertexPrg),
	  std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, "#version 460\n#define FRAGMENT_SHADER\n" + fragmentPrg)
  );

  vertexPrg = readFileToString(std::string(PROJECT_DEP) + "/shaders/FinalRender.vs");
  fragmentPrg = readFileToString(std::string(PROJECT_DEP) + "/shaders/FinalRender.fs");

  vars.reCreate<ge::gl::Program>("method.FinalRender_s",
	  std::make_shared<ge::gl::Shader>(GL_VERTEX_SHADER, "#version 460\n#define   VERTEX_SHADER\n" + vertexPrg),
	  std::make_shared<ge::gl::Shader>(GL_FRAGMENT_SHADER, "#version 460\n#define FRAGMENT_SHADER\n" + fragmentPrg)
  );
  
  glCreateVertexArrays(5, vao);
  for (int i = 0; i < 5; i++) {
	  for (int j = 0; j < objects[i].size(); j += 3) {
		  addNormals(&objects[i][j], &objects[i][j + 1], &objects[i][j + 2], (j / 3) + 1);
		  if (i < 3) {
			  for (int k = 0; k < 3; k++) {
				  objects[i][j + k].nx = objects[i][j + k].nx * -1.f;
				  objects[i][j + k].ny = objects[i][j + k].ny * -1.f;
				  objects[i][j + k].nz = objects[i][j + k].nz * -1.f;
			  }
		  }
	  }
	  
	  vbo[i] = createbuffer(objects[i]);

	  addAttrib(vao[i], 0, 3, GL_FLOAT, sizeof(float) * 0, sizeof(TextureVertex), vbo[i]);
	  addAttrib(vao[i], 1, 2, GL_FLOAT, sizeof(float) * 3, sizeof(TextureVertex), vbo[i]);
	  addAttrib(vao[i], 2, 3, GL_FLOAT, sizeof(float) * 5, sizeof(TextureVertex), vbo[i]);
	  addAttrib(vao[i], 3, 3, GL_FLOAT, sizeof(float) * 8, sizeof(TextureVertex), vbo[i]);
  }

  tex[0] = createTexture(std::string(PROJECT_DEP) + "/textures/red.png");
  tex[1] = createTexture(std::string(PROJECT_DEP) + "/textures/green.png");
  tex[2] = createTexture(std::string(PROJECT_DEP) + "/textures/blue.png");
  tex[3] = createTexture(std::string(PROJECT_DEP) + "/textures/pyramid_yellow.png");
  tex[4] = createTexture(std::string(PROJECT_DEP) + "/textures/grey.png");

  glCreateVertexArrays(1, &lightCubeAO);

  for (int i = 0; i < lightCube.size(); i += 3) {
	  addNormals(&lightCube[i], &lightCube[i + 1], &lightCube[i + 2], i / 3 + 1);
	  if (i > 23 && i < 30) {
		  lightCube[i].ny = lightCube[i].ny * -1.f;
		  lightCube[i+1].ny = lightCube[i+1].ny * -1.f;
		  lightCube[i+2].ny = lightCube[i+2].ny * -1.f;
	  }
  }

  lightCubeBO = createbuffer(lightCube);

  addAttrib(lightCubeAO, 0, 3, GL_FLOAT, sizeof(float) * 0, sizeof(TextureVertex), lightCubeBO);
  addAttrib(lightCubeAO, 1, 2, GL_FLOAT, sizeof(float) * 3, sizeof(TextureVertex), lightCubeBO);
  addAttrib(lightCubeAO, 2, 3, GL_FLOAT, sizeof(float) * 5, sizeof(TextureVertex), lightCubeBO);
  addAttrib(lightCubeAO, 3, 3, GL_FLOAT, sizeof(float) * 8, sizeof(TextureVertex), lightCubeBO);

  lightCubeTex = createTexture(std::string(PROJECT_DEP) + "/textures/white.png");

  auto SAVBO = vars.reCreate<Buffer>("method.screenAlignedVBO", sizeof(GLfloat) * screenAlignedQuadVertices.size(), screenAlignedQuadVertices.data());
  auto SAVAO = vars.reCreate<VertexArray>("method.screenAlignedVAO");

  SAVAO->addAttrib(SAVBO, 0, 2, GL_FLOAT, 4 * sizeof(float), 0);
  SAVAO->addAttrib(SAVBO, 1, 2, GL_FLOAT, 4 * sizeof(float), 2 * sizeof(float));

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glEnable(GL_DEPTH_TEST);
}

void onDraw(vars::Vars&vars){

  ImGui::Begin("vars");
  if (ImGui::Button("start preprocess and radiosity iteration")) {
	  if (preprocessDone == 0) {
		  preprocessDone++;
		  doRadiosityIteration = true;
	  }
  }
  if (ImGui::Button("start iteration")) {
	  if (preprocessDone == 2) {
		  doRadiosityIteration = true;
	  }
  }
  if (ImGui::Button("stop iteration")) {
	  if (preprocessDone == 2) {
		  doRadiosityIteration = false;
	  }
  }
  if (ImGui::Button("only texture")) {
	  preprocessDone = 0;
	  doRadiosityIteration = false;
  }
  ImGui::End();

  computeProjectionMatrix(vars);

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  auto proj = vars.get<basicCamera::PerspectiveCamera>("method.proj")->getProjection();
  auto view = vars.get<basicCamera::OrbitCamera>("method.view")->getView();
  auto prg = vars.get<ge::gl::Program>("method.prg");
  
  if (preprocessDone > 0) {

	  prg = vars.get<ge::gl::Program>("method.FinalRender_s");
	  auto model = glm::mat4(1);
	  auto mapSize = (GLsizei)pow(2, vars.getUint32("method.radiosity.RadiosityTexture_size"));

	  prg
		  ->setMatrix4fv("projection", glm::value_ptr(proj))
		  ->setMatrix4fv("view", glm::value_ptr(view))
		  ->set1i("addAmbient", 1)
		  ->use();

	  if (vars.getBool("method.radiosity.addAmbient")) {
		  prg->set1i("addAmbient", 1)->use();
	  }
	  else {
		  prg->set1i("addAmbient", 0)->use();
	  }

	  if (preprocessDone == 1) {
		  preprocess(vars);
		  std::cout << "Preprocess done" << std::endl;
		  preprocessDone++;
	  }

	  glm::vec3 shooterRadiance;
	  glm::vec3 shooterWorldspacePos;
	  glm::vec3 shooterWorldspaceNormal;
	  glm::vec3 shooterUV3D;

	  if (doRadiosityIteration) {
		  if (meshSelectionNeeded) {
			  shooterMesh = selectShooterMesh(vars);
			  meshSelectionNeeded = false;
		  }
		  
			selectMeshBasedShooter(vars, shooterRadiance, shooterWorldspacePos, shooterWorldspaceNormal, shooterUV3D, shooterMesh);

			std::vector<glm::mat4> shooterViews;

			unsigned int visibilityTextureSize = 1024;

			glm::mat4 shooterProj = glm::perspective(glm::radians(90.f), 1.f, 0.1f, 100.f);

			prg = vars.get<ge::gl::Program>("method.HemicubeVisTex_s");
			prg->use();
			prg->setMatrix4fv("projection", glm::value_ptr(shooterProj));

			std::vector<unsigned int> visibilityTextures = createHemicubeTextures(vars, shooterViews, visibilityTextureSize, shooterWorldspacePos, shooterWorldspaceNormal);

			glm::vec2 shooterUV = glm::vec2(shooterUV3D.x, shooterUV3D.y);


			prg = vars.get<ge::gl::Program>("method.LightmapUpdate_s");
			prg->use();
			GLint loc = prg->getUniformLocation("projection");
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(shooterProj));
			loc = prg->getUniformLocation("shooterRadiance");
			glUniform3fv(loc, 1, glm::value_ptr(shooterRadiance));
			loc = prg->getUniformLocation("shooterWorldspacePos");
			glUniform3fv(loc, 1, glm::value_ptr(shooterWorldspacePos));
			loc = prg->getUniformLocation("shooterWorldspaceNormal");
			glUniform3fv(loc, 1, glm::value_ptr(shooterWorldspaceNormal));
			loc = prg->getUniformLocation("shooterUV");
			glUniform2fv(loc, 1, glm::value_ptr(shooterUV));


			updateLighmaps(vars, shooterViews, visibilityTextures);

			glDeleteTextures(5, &visibilityTextures[0]);
	  }
		prg = vars.get<ge::gl::Program>("method.FinalRender_s");
		prg->use();

		for (int i = 0; i < 6; i++) {
			if (i == 5) {
				glm::mat4 lampModel = glm::mat4(1);
				glm::vec3 lightPos = *vars.get<glm::vec3>("method.light.LightPos");
				float scale = vars.getFloat("method.light.LightCube_size");

				lampModel = glm::translate(lampModel, lightPos);
				lampModel = glm::scale(lampModel, glm::vec3(scale));

				prg->setMatrix4fv("model", glm::value_ptr(lampModel));

				unsigned int irradianceID;
				glGenTextures(1, &irradianceID);

				glBindTexture(GL_TEXTURE_2D, irradianceID);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mapSize, mapSize, 0, GL_RGB, GL_FLOAT, &meshes[i].irradianceData[0]);

				//There is no point trying to filter a lamp since it is equally bright everywhere
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				glActiveTexture(GL_TEXTURE0);
				glUniform1i(prg->getAttribLocation("irradianceTexture"), 0);
				glBindTexture(GL_TEXTURE_2D, irradianceID);

				glBindVertexArray(lightCubeAO);
				glBindTextureUnit(1, lightCubeTex);
				glDrawArrays(GL_TRIANGLES, 0, (GLsizei)lightCube.size());

				glDeleteTextures(1, &irradianceID);
			}
			else
			{
				prg->setMatrix4fv("model", glm::value_ptr(model));

				unsigned int irradianceID;
				glGenTextures(1, &irradianceID);

				glBindTexture(GL_TEXTURE_2D, irradianceID);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mapSize, mapSize, 0, GL_RGB, GL_FLOAT, &meshes[i].irradianceData[0]);

				if (vars.getBool("method.radiosity.textureFiltering")) {
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				}
				else {
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				}

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				glActiveTexture(GL_TEXTURE0);
				glUniform1i(prg->getAttribLocation("irradianceTexture"), 0);
				glBindTexture(GL_TEXTURE_2D, irradianceID);

				glBindVertexArray(vao[i]);
				glBindTextureUnit(1, tex[i]);
				glDrawArrays(GL_TRIANGLES, 0, (GLsizei)objects[i].size());

				glDeleteTextures(1, &irradianceID);
			} 
	  }

  }
  else {
	  prg = vars.get<ge::gl::Program>("method.prg");
	  prg->setMatrix4fv("proj", glm::value_ptr(proj))
		  ->setMatrix4fv("view", glm::value_ptr(view))
		  ->set1f("lightCube_size", vars.getFloat("method.light.LightCube_size"))
		  ->set3fv("lightPos", glm::value_ptr(*vars.get<glm::vec3>("method.light.LightPos")))
		  ->set1i("switcher", 0)
		  ->use();

	  for (int i = 0; i < 5; i++) {
		  glBindVertexArray(vao[i]);
		  glBindTextureUnit(0, tex[i]);
		  glDrawArrays(GL_TRIANGLES, 0, (GLsizei)objects[i].size());
	  }

	  prg
		  ->set1i("switcher", 1);

	  glBindVertexArray(lightCubeAO);
	  glBindTextureUnit(0, lightCubeTex);
	  glDrawArrays(GL_TRIANGLES, 0, (GLsizei)lightCube.size());
  }
}

void onResize(vars::Vars&vars){
  /// size of the screen
  auto width  = vars.getUint32("event.resizeX");
  auto height = vars.getUint32("event.resizeY");

  glViewport(0,0,width,height);
}

void onQuit(vars::Vars&vars){
  /// if you created everything inside "method" namespace
  /// this line should be enough to clear everything...
  glDeleteTextures(5, tex);
  glDeleteTextures(1, &lightCubeTex);
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
