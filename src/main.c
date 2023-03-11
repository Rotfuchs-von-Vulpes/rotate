#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cglm/cglm.h>

unsigned int VBO;
unsigned int VAO;
unsigned int EBO;

const char *vertexShaderSource =
	"#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"layout (location = 1) in vec3 aColor;\n"
	"layout (location = 2) in vec2 aTexCoord;\n"
	"out vec3 ourColor;\n"
	"out vec2 TexCoord;\n"
	"uniform mat4 transform;\n"
	"void main()\n"
	"{\n"
	"  gl_Position = transform*vec4(aPos, 1.0);\n"
	"  TexCoord = aTexCoord.xy;\n"
	"}\0";

const char *fragmentShaderSource =
	"#version 330 core\n"
	"out vec4 FragColor;\n"
	"in vec3 ourColor;\n"
	"in vec2 TexCoord;\n"
	"uniform sampler2D texture1;\n"
	"void main()\n"
	"{\n"
	" FragColor = texture(texture1, TexCoord);\n"
	"}";

unsigned int shaderProgram[2];
unsigned int texture1, texture2;
unsigned int texture;
int textureCount = 1;
int shaderCount = 0;

void loadShader(unsigned int vertexShader, unsigned int fragmentShader)
{
	shaderProgram[shaderCount] = glCreateProgram();

	glAttachShader(shaderProgram[shaderCount], vertexShader);
	glAttachShader(shaderProgram[shaderCount], fragmentShader);
	glLinkProgram(shaderProgram[shaderCount]);

	glUseProgram(shaderProgram[shaderCount]);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);  
	char infoLog[512];
	glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
	printf("%s\n", infoLog);

	++shaderCount;
}

unsigned int loadTexture(const char *image)
{
	glGenTextures(++textureCount, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); 
	unsigned char *data = stbi_load(image, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		puts("Failed to load texture");
	}
	stbi_image_free(data);

	return texture;
}

void init() {
	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	glGenVertexArrays(1, &VAO); 
	glGenBuffers(1, &VBO); 
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	loadShader(vertexShader, fragmentShader);
	loadShader(vertexShader, fragmentShader);

	texture1 = loadTexture("default_wood.png");
	texture2 = loadTexture("default_sand.png");

	glUseProgram(shaderProgram[0]);
	glUniform1i(glGetUniformLocation(shaderProgram[0], "texture"), 0);
	glUseProgram(shaderProgram[1]);
	glUniform1i(glGetUniformLocation(shaderProgram[1], "texture"), 1);
	
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void render() {
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgram[0]);
	mat4 trans = GLM_MAT4_IDENTITY_INIT;
	glm_translate(trans, (vec2){-0.5, 0});
	glm_rotate(trans, (float)glfwGetTime(), (vec3){0.0, 0.0, 1.0});
	glm_scale(trans, (vec3){0.5, 0.5, 0.5});

	unsigned int transformLoc = glGetUniformLocation(shaderProgram[0], "transform");
	glUniformMatrix4fv(transformLoc, 1, false, (float *)trans);

	glBindTexture(GL_TEXTURE_2D, texture1);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glUseProgram(shaderProgram[1]);
	glm_mat4_identity(trans);
	glm_translate(trans, (vec2){0.5, 0});
	glm_rotate(trans, -(float)glfwGetTime(), (vec3){0.0, 0.0, 1.0});
	glm_scale(trans, (vec3){0.5, 0.5, 0.5});

	transformLoc = glGetUniformLocation(shaderProgram[1], "transform");
	glUniformMatrix4fv(transformLoc, 1, false, (float *)trans);

	glBindTexture(GL_TEXTURE_2D, texture2);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int main(int argc, char **args)
{
	GLFWwindow* window;

	int screen_width = 600;
	int screen_height = 600;

	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(screen_width, screen_height, "Textures test", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	printf("OpenGL loaded\nVersion: %s", glGetString(GL_VERSION));


	init();
	while (!glfwWindowShouldClose(window))
	{
		render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}