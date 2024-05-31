#include<Vars/Vars.h>
#include<geGL/StaticCalls.h>
#include<framework/methodRegister.hpp>
#include<PGR/01/emptyWindow.hpp>
#include<PGR/01/compileShaders.hpp>
#include<PGR/01/vertexArrays.hpp>
#include<framework/makeProgram.hpp>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/geometric.hpp>

using namespace ge::gl;
using namespace emptyWindow;
using namespace compileShaders;

namespace pgr_task01 {

	GLuint vao;
	GLuint vbo;

	glm::mat4 view;
	glm::mat4 proj;

	float time = 0.f;

	const float radius = 5.f;

	std::shared_ptr<Program>prg;
	//std::shared_ptr<Buffer>vbo;
	template<typename T>
	GLuint createBuffer(std::vector<T> const& data) {
		glCreateBuffers(1, &vbo);
		glNamedBufferData(vbo, vertices.size() * sizeof(decltype(vertices)::value_type), vertices.data(), GL_DYNAMIC_DRAW);
		return vbo;
	}
	void addAttrib(GLuint vao, GLint a, GLint n, GLenum t, GLsizei o, GLsizei s) {
		glVertexArrayAttribBinding(vao, a, a);
		glEnableVertexArrayAttrib(vao, a);
		glVertexArrayAttribFormat(vao, a, n, t, GL_FALSE, 0);
		glVertexArrayVertexBuffer(vao, a, vbo, o, s);
	}

	struct Vertex {
		float x, y, z;
		float r, g, b;
	};

	std::vector<Vertex>vertices = {
		//Lava stena
		{-2,0,-2,1,0,0},
		{-2,0,-3,1,0,0},
		{-2,2,-2,1,0,0},

		{-2,2,-2,1,0,0},
		{-2,0,-3,1,0,0},
		{-2,2,-3,1,0,0},


		{-2,0,0,1,0,0},
		{-2,0,-2,1,0,0},
		{-2,1,0,1,0,0},

		{-2,1,0,1,0,0},
		{-2,0,-2,1,0,0},
		{-2,1,-2,1,0,0},


		{-2,1,0,1,0,0},
		{-2,1,-1,1,0,0},
		{-2,3,0,1,0,0},

		{-2,3,0,1,0,0},
		{-2,1,-1,1,0,0},
		{-2,3,-1,1,0,0},


		{-2,2,-1,1,0,0},
		{-2,2,-3,1,0,0},
		{-2,3,-1,1,0,0},

		{-2,3,-1,1,0,0},
		{-2,2,-3,1,0,0},
		{-2,3,-3,1,0,0},

		//Prava stena
		{1,0,-2,1,0,1},
		{1,0,-3,1,0,1},
		{1,2,-2,1,0,1},

		{1,2,-2,1,0,1},
		{1,0,-3,1,0,1},
		{1,2,-3,1,0,1},


		{1,0,0,1,0,1},
		{1,0,-2,1,0,1},
		{1,1,0,1,0,1},

		{1,1,0,1,0,1},
		{1,0,-2,1,0,1},
		{1,1,-2,1,0,1},


		{1,1,0,1,0,1},
		{1,1,-1,1,0,1},
		{1,3,0,1,0,1},

		{1,3,0,1,0,1},
		{1,1,-1,1,0,1},
		{1,3,-1,1,0,1},


		{1,2,-1,1,0,1},
		{1,2,-3,1,0,1},
		{1,3,-1,1,0,1},

		{1,3,-1,1,0,1},
		{1,2,-3,1,0,1},
		{1,3,-3,1,0,1},

		//Predna stena
		{-2,0,0,0,1,0},
		{-1,0,0,0,1,0},
		{-2,2,0,0,1,0},

		{-2,2,0,0,1,0},
		{-1,0,0,0,1,0},
		{-1,2,0,0,1,0},


		{-1,0,0,0,0,1},
		{0,0,0,0,0,1},
		{-1,2,0,0,0,1},

		{-1,2,0,0,0,1},
		{0,0,0,0,0,1},
		{0,2,0,0,0,1},


		{0,0,0,0,1,0},
		{1,0,0,0,1,0},
		{0,2,0,0,1,0},

		{0,2,0,0,1,0},
		{1,0,0,0,1,0},
		{1,2,0,0,1,0},


		{-2,2,0,0,1,0},
		{1,2,0,0,1,0},
		{-2,3,0,0,1,0},

		{-2,3,0,0,1,0},
		{1,2,0,0,1,0},
		{1,3,0,0,1,0},

		//Zadna stena
		{-2,0,-3,0,1,1},
		{1,0,-3,0,1,1},
		{-2,3,-3,0,1,1},

		{-2,3,-3,0,1,1},
		{ 1,0,-3,0,1,1},
		{1,3,-3,0,1,1},

		//Strecha
		{ -2,3,0,1,1,0 },
		{ 1,3,0,1,1,0 },
		{ -0.5,4,-1.5f,1,1,0 },

		{ -1,3,0,1,1,0 },
		{ 1,3,-3,1,1,0 },
		{ -0.5f,4,-1.5f,1,1,0 },

		{ 1,3,-3,1,1,0 },
		{ -2,3,-3,1,1,0 },
		{ -0.5f,4,-1.5f,1,1,0 },

		{ -2,3,-3,1,1,0 },
		{ -2,3,0,1,1,0 },
		{ -0.5,4,-1.5f,1,1,0 },

		//Komin
		{ -1.75f,3.10f,-1.5f,1,1,1 },
		{ -1.5f,3.25f,-1.5f,1,1,1 },
		{ -1.75f,4.25f,-1.5f,1,1,1 },

		{ -1.75f,4.25f,-1.5f,1,1,1 },
		{ -1.5f,3.25f,-1.5f,1,1,1 },
		{ -1.5f,4.25f,-1.5f,1,1,1 },

		{ -1.67f,3.17f,-1.32f,1,1,1 },
		{ -1.67f,3.17f,-1.67f,1,1,1 },
		{ -1.67f,4.25f,-1.32f,1,1,1 },

		{ -1.67f,4.25f,-1.32f,1,1,1 },
		{ -1.67f,3.17f,-1.67f,1,1,1 },
		{ -1.67f,4.25f,-1.67f,1,1,1 },

		//Zem
		{ -6,0,5,144.f / 255.f,238.f / 255.f,144.f / 255.f },
		{ 5,0,5,144.f / 255.f,238.f / 255.f,144.f / 255.f },
		{ -6,0,-6,144.f / 255.f,238.f / 255.f,144.f / 255.f },

		{ -6,0,-6,144.f / 255.f,238.f / 255.f,144.f / 255.f },
		{ 5,0,5,144.f / 255.f,238.f / 255.f,144.f / 255.f },
		{ 5,0,-6,144.f / 255.f,238.f / 255.f,144.f / 255.f },

		//Podlaha domu
		{ -2,0.01f,0,160.f / 255.f,122.f / 255.f,85.f / 255.f },
		{ 1,0.01f,0,160.f / 255.f,122.f / 255.f,85.f / 255.f },
		{ -2,0.01f,-3,160.f / 255.f,122.f / 255.f,85.f / 255.f },

		{ -2,0.01f,-3,160.f / 255.f,122.f / 255.f,85.f / 255.f },
		{ 1,0.01f,0,160.f / 255.f,122.f / 255.f,85.f / 255.f },
		{ 1,0.01f,-3,160.f / 255.f,122.f / 255.f,85.f / 255.f },

		//Cesta
		{ -1,0.01f,5,210.f / 255.f,209.f / 255.f,205.f / 255.f },
		{ 0,0.01f,5,210.f / 255.f,209.f / 255.f,205.f / 255.f },
		{ -1,0.01f,0,210.f / 244.f,209.f / 255.f,205.f / 255.f },

		{ -1,0.01f,0,210.f / 255.f,209.f / 255.f,205.f / 255.f },
		{ 0,0.01f,5,210.f / 255.f,209.f / 255.f,205.f / 255.f },
		{ 0,0.01f,0,210.f / 244.f,209.f / 255.f,205.f / 255.f },

	};

	void onResize(vars::Vars& vars) {
		auto width = vars.getUint32("event.resizeX");
		auto height = vars.getUint32("event.resizeY");

		glViewport(0, 0, width, height);

		float a = (float)width / (float)height;
		proj = glm::perspective(glm::radians(90.f), a, .1f, 1000.f);
	}

	void changeLookAt(vars::Vars& vars) {
		float camX = sin(time) * radius;
		float camZ = cos(time) * radius;

		view = glm::lookAt(glm::vec3(camX, 3.f, camZ), glm::vec3(0.f, 3.f, 0.0f), glm::vec3(0.f, 1.f, 0.f));
	}

	void onInit(vars::Vars& vars) {
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.2f, 0.2f, 1.0f);

		//vbo = std::make_shared<Buffer>(vertices);


		vbo = createBuffer(vertices);
		glCreateVertexArrays(1, &vao);
		addAttrib(vao, 0, 3, GL_FLOAT, sizeof(float) * 0, sizeof(Vertex));
		addAttrib(vao, 1, 3, GL_FLOAT, sizeof(float) * 3, sizeof(Vertex));

		auto src = R".(
	#ifdef VERTEX_SHADER

	layout(location=0)in vec3 position;
	layout(location=1)in vec3 color;

	uniform mat4 view = mat4(1);
	uniform mat4 proj = mat4(1);

	out vec4 Vcolor;
	void main(){
		gl_Position = proj * view * vec4(position,1);
		Vcolor = vec4(color,1);
	}
	#endif

	#ifdef FRAGMENT_SHADER
	in vec4 Vcolor;
	out vec4 FColor;
	void main(){
		FColor = Vcolor;
	}
	#endif
	).";
		prg = makeProgram(src);
		prg->setNonexistingUniformWarning(false);

		onResize(vars);
		changeLookAt(vars);
	}

	void onDraw(vars::Vars& vars) {
		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		time += 0.0005f;
		if (time >= 1000000000.f) {
			time = 0.f;
		}

		prg->use();

		changeLookAt(vars);

		prg->setMatrix4fv("proj", glm::value_ptr(proj));
		prg->setMatrix4fv("view", glm::value_ptr(view));

		glBindVertexArray(vao);

		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
	}

	void onQuit(vars::Vars&) {

	}

	EntryPoint main = []() {
		methodManager::Callbacks clbs;
		clbs.onInit = onInit;
		clbs.onQuit = onQuit;
		clbs.onDraw = onDraw;
		clbs.onResize = onResize;
		MethodRegister::get().manager.registerMethod("pgr01.task", clbs);
		};

}