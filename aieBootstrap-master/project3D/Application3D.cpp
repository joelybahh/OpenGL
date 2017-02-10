

/*
-------------------------------------------------------------------------------
INSTRUCTIONS:
-------------------------------------------------------------------------------
STEP 1: Load a shader program
See the LoadShader method

STEP 2: Generate Geometry
See the CreateGeometry method

STEP 3: Each Frame - Render Geometry (using the shader program)
See the DrawGeometry method

STEP 4: Unload Shader and Geometry
-------------------------------------------------------------------------------
*/

#include "Application3D.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "gl_core_4_4.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

Application3D::Application3D() {

}

Application3D::~Application3D() {

}

bool Application3D::startup() {
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);

	LoadShader();
	CreateCube();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void Application3D::shutdown() {
	DestroyCube();
	UnloadShader();

	Gizmos::destroy();
}

void Application3D::update(float deltaTime) {

	// query time since application started
	float time = getTime();

	// rotate camera
	m_viewMatrix = glm::lookAt(vec3(glm::sin(time) * 10, 10, glm::cos(time) * 10),
							   vec3(0), vec3(0, 1, 0));

	// wipe the gizmos clean for this frame
	Gizmos::clear();

	DrawGird();


	mat4 t = glm::rotate(time, glm::normalize(vec3(1, 1, 1)));
	t[3] = vec4(-2, 0, 0, 1);
	

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void Application3D::DrawGird()
{
	// draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
			vec3(-10 + i, 0, -10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
			vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}
}

void Application3D::draw() {
	
	// wipe the screen to the background colour
	clearScreen();

	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);

	// STEP 1: enable the shader program for rendering
	glUseProgram(m_shader);

	// Step 2: send uniform variables to the shader
	glm::mat4 projectionView = m_projectionMatrix * m_viewMatrix;
	glUniformMatrix4fv(
		glGetUniformLocation(m_shader, "projectionView"), 
		1, 
		false, 
		glm::value_ptr(projectionView));

	// Step 3: Bind the VAO
	// When we setup the geometry, we did a bunch of glEnableVertexAttribArray and glVertexAttribPointer method calls
	// we also Bound the vertex array and index array via the glBindBuffer call.
	// if we where not using VAO's we would have to do thoes method calls each frame here.
	glBindVertexArray(m_cubeVao);

	// Step 4: Draw Elements. We are using GL_TRIANGLES.
	// we need to tell openGL how many indices there are, and the size of our indices
	// when we setup the geometry, our indices where an unsigned char (1 byte for each indicy)
	glDrawElements(GL_TRIANGLES, m_cubeIndicesCount, GL_UNSIGNED_BYTE, 0);

	// Step 5: Now that we are done drawing the geometry
	// unbind the vao, we are basicly cleaning the opengl state
	glBindVertexArray(0);

	// Step 6: de-activate the shader program, dont do future rendering with it any more.
	glUseProgram(0);

	Gizmos::draw(m_projectionMatrix * m_viewMatrix);
}


void Application3D::LoadShader()
{
	static const char* vertex_shader =
	"#version 400\n									\
	in vec4 vPosition;\n							\
	in vec4 vColor;\n								\
	out vec4 fColor;\n								\
	uniform mat4 projectionView; \n					\
	void main ()\n									\
	{\n												\
	  fColor = vColor;\n							\
	  gl_Position = projectionView * vPosition;\n	\
	}";

	static const char* fragment_shader =
	"#version 400\n				\
	in vec4 fColor;\n			\
	out vec4 frag_color;\n		\
	void main ()\n				\
	{\n							\
	  frag_color = fColor;\n	\
	}";

	// Step 1:
	// Load the vertex shader, provide it with the source code and compile it.
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);

	// Step 2:
	// Load the fragment shader, provide it with the source code and compile it.
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);

	// step 3:
	// Create the shader program
	m_shader = glCreateProgram();

	// Step 4:
	// attach the vertex and fragment shaders to the m_shader program
	glAttachShader(m_shader, vs);
	glAttachShader(m_shader, fs);

	// Step 5:
	// describe the location of the shader inputs the link the program
	glBindAttribLocation(m_shader, 0, "vPosition");
	glBindAttribLocation(m_shader, 1, "vColor");
	glLinkProgram(m_shader);

	// step 6:
	// delete the vs and fs shaders
	glDeleteShader(vs);
	glDeleteShader(fs);


}
void Application3D::UnloadShader()
{
	glDeleteProgram(m_shader);
}

void Application3D::CreateCube()
{
	Vertex verts[] = {

		// POSITION						COLOR
		// FRONT FACE				  - RED
		{ { -0.5f,-0.5f, 0.5f, 1.0f },{ 1.0f, 0.0f, 0.0f, 0.5f } },	// 0
		{ { 0.5f,-0.5f, 0.5f, 1.0f },{ 0.0f, 1.0f, 0.0f, 0.5f } },	// 1
		{ { 0.5f, 0.5f, 0.5f, 1.0f },{ 1.0f, 0.0f, 1.0f, 0.5f } },	// 2
		{ { -0.5f, 0.5f, 0.5f, 1.0f },{ 1.0f, 1.0f, 1.0f, 0.5f } },	// 3

		// BACK_FACE					    GREEN
		{ { -0.5f, -0.5f, -0.5f, 1.0f } , { 1.0f, 0.0f, 0.0f, 1.0f } }, // 4
		{ {  0.5f, -0.5f, -0.5f, 1.0f } , { 0.0f, 1.0f, 0.0f, 1.0f } }, // 5
		{ {  0.5f,  0.5f, -0.5f, 1.0f } , { 0.0f, 0.0f, 1.0f, 1.0f } }, // 6
		{ { -0.5f,  0.5f, -0.5f, 1.0f } , { 1.0f, 1.0f, 1.0f, 1.0f } }, // 
	};

	unsigned char indices[] = {
		0, 1, 2,	0, 2, 3,			// front face
		5, 4, 7,    5, 7, 6,
		1, 5, 6,    1, 6, 2,
		4, 0, 3,    4, 3, 7,
		3, 2, 6,    3, 6, 7
	};

	m_cubeIndicesCount = sizeof(indices) / sizeof(unsigned char);

	// Generate the VAO and Bind bind it.
	// Our VBO (vertex buffer object) and IBO (Index Buffer Object) will be "grouped" with this VAO
	// other settings will also be grouped with the VAO. this is used so we can reduce draw calls in the render method.
	glGenVertexArrays(1, &m_cubeVao);
	glBindVertexArray(m_cubeVao);

	// Create our VBO and IBO.
	// Then tell Opengl what type of buffer they are used for
	// VBO a buffer in graphics memory to contains our vertices
	// IBO a buffer in graphics memory to contain our indices.
	// Then Fill the buffers with our generated data.
	// This is taking our verts and indices from ram, and sending them to the graphics card
	glGenBuffers(1, &m_cubeVbo);
	glGenBuffers(1, &m_cubeIbo);

	glBindBuffer(GL_ARRAY_BUFFER, m_cubeVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	Vertex::SetupVertexAttribPointers();

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Application3D::Vertex::SetupVertexAttribPointers()
{
	// enable vertex position element
	// notice when we loaded the shader, we described the "position" element to be location 0.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0 (position)
		4,                  // size - how many floats make up the position (x, y, z, w)
		GL_FLOAT,           // type - our x,y,z, w are float values
		GL_FALSE,           // normalized? - not used
		sizeof(Vertex),     // stride - size of an entire vertex
		(void*)0            // offset - bytes from the beginning of the vertex
	);

	// enable vertex color element
	// notice when we loaded the shader, we described the "color" element to be location 1.
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                  // attribute 1 (color)
		4,                  // size - how many floats make up the color (r, g, b, a)
		GL_FLOAT,           // type - our x,y,z are float values
		GL_FALSE,           // normalized? - not used
		sizeof(Vertex),     // stride - size of an entire vertex
		(void*)(sizeof(float) * 4)            // offset - bytes from the beginning of the vertex
	);
}

void Application3D::DestroyCube()
{
	// When We're Done, destroy the geometry
	glDeleteBuffers(1, &m_cubeIbo);
	glDeleteBuffers(1, &m_cubeVbo);
	glDeleteVertexArrays(1, &m_cubeVao);
}
