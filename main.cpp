#include <GL/glew.h>
//#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <iostream>

#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof((x)[0]))
#endif

//keep track of window size for things like the viewport and the mouse cursor
int g_gl_width;
int g_gl_height;

void print_program_info_log(GLuint program){
	int max_length = 2048;
	int actual_length = 0;
	char program_log[2048];
	glGetProgramInfoLog(program, max_length, &actual_length, program_log);
	printf("program info log for GL index %u:\n%s", program, program_log);
}

bool is_valid(GLuint program){
	glValidateProgram(program);
	int params = -1; 
	glGetProgramiv(program, GL_VALIDATE_STATUS, &params);
	printf("program %i GL_VALIDATE_STATUS = %i\n", program, params);
	if(GL_TRUE != params){
		print_program_info_log(program);
		return false;
	}
	return true;
}

bool shader_link_check(GLuint program){
	int params = -1;
	glGetProgramiv(program, GL_LINK_STATUS, &params);
	if(GL_TRUE != params){
		fprintf(stderr, "ERROR: could not link shader program GL index %u\n", program);
		return false;
	}
	return true;
}

void print_shader_info_log(GLuint shader_index){
	int max_length = 2048;
	int actual_length = 0;
	char shader_log[2048];
	glGetShaderInfoLog(shader_index, max_length, &actual_length, shader_log);
	printf("shader info log for GL index %u:\n%s\n", shader_index, shader_log);
}

bool load_shader(const char* filename, char* &string){
	//read shaders from file
	FILE* f = fopen(filename, "r");

	if(!f){
		fprintf(stderr, "ERROR: %s not opened", filename);
		return false;
	}

	//Determine file size
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);

	//calloc() initialises memory to zero
	string = (char*)calloc(sizeof(char), size + 1);

	rewind(f);
	fread((void*)string, sizeof(char), size, f);
	*(string + size + 1) = '\0';
	printf("%s\n", string);
	return true;
}

bool shader_compile_check(GLuint shader){
	int params = -1;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &params);
	if(GL_TRUE != params){
		fprintf(stderr, "ERROR: GL shader %i did not compile\n", shader);
		print_shader_info_log(shader);
		return false;
	}
	return true;
}

int main(int argc, char **argv){

    std::cout << "Glew Test" << std::endl;

	//start gl context and O/S window using the glfw helper library
	if(!glfwInit()){
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	//uncomment these lines if on osx
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//anti-aliasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	g_gl_width = 640;//vmode->width;
	g_gl_height = 480;//vmode->height;

	GLFWwindow* window = glfwCreateWindow(g_gl_width, g_gl_height, "Extended GL Init", NULL, NULL);
	if(!window){
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	//start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	//get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); //get renderer string
	const GLubyte* version = glGetString(GL_VERSION); //version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
//**************************************************************************************************
//	Ground Plane Setup
//*********************************************************************************************
	// triangle vertices
	float triangleVerts [9] = {
		0.0f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
	};

	//Set up ground plane buffers
	GLuint triVAO;
	glGenVertexArrays(1, &triVAO);
	glBindVertexArray(triVAO);

	GLuint triVBO;
	glGenBuffers(1, &triVBO);
	glBindBuffer(GL_ARRAY_BUFFER, triVBO);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), triangleVerts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//load shaders
	char* triVertShader;
	bool isTriVertLoaded = load_shader("tri.vert", triVertShader);
	if(!isTriVertLoaded) return 1;

	char* triFragShader;
	bool isTriFragLoaded = load_shader("tri.frag", triFragShader);
	if(!isTriFragLoaded) return 1;

	GLuint tvs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(tvs, 1, &triVertShader, NULL);
	glCompileShader(tvs);
	delete[] triVertShader;
	//check for compile errors
	bool isTriVertCompiled = shader_compile_check(tvs);
	if(!isTriVertCompiled) return 1;

	GLuint tfs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(tfs, 1, &triFragShader, NULL);
	glCompileShader(tfs);
	delete[] triFragShader;
	//check for compile errors
	bool isTriFragCompiled = shader_compile_check(tfs);
	if(!isTriFragCompiled) return 1;
	
	GLuint triShaderProg = glCreateProgram();
	glAttachShader(triShaderProg, tfs);
	glAttachShader(triShaderProg, tvs);
	glLinkProgram(triShaderProg);
	bool didTriShadersLink = shader_link_check(triShaderProg);
	if(!didTriShadersLink) return 1;

	//only use during development as computationally expensive
	bool validGroundProgram = is_valid(triShaderProg);
	if(!validGroundProgram){
		fprintf(stderr, "ERROR: triShaderProg not valid\n");
		return 1;
	}

	glBindVertexArray(0);

//***************************************************************************************************		
	//workaround for macOS Mojave bug
	bool needDraw = true;

	while(!glfwWindowShouldClose(window)){

//***********************************************************************************************************
// Update Stuff Here
//*********************************************************************************************************
		//wipe the drawing surface clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						
//**********************************************************************************************************
// Draw Stuff Here
//*********************************************************************************************************
		
		glUseProgram(triShaderProg);
		glBindVertexArray(triVAO);
		glBindBuffer(GL_ARRAY_BUFFER, triVBO); 
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		
		// workaround for macOS Mojave bug
		if(needDraw){
			glfwShowWindow(window);
			glfwHideWindow(window);
			glfwShowWindow(window);
			needDraw = false;
		}

		glfwPollEvents();
		//put the stuff we've been drawing onto the display
		glfwSwapBuffers(window);
	}


	//close GL context and any other GL resources
	glfwTerminate();
	return 0;
}
