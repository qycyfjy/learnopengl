#include <cstdio>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

void OnWindowResize(GLFWwindow* window, int width, int height) {
	printf("current window size: %d:%d\n", width, height);
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr) {
		printf("failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, OnWindowResize);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("failed to init GLAD\n");
		return -1;
	}

	glViewport(0, 0, 800, 600);

	while (!glfwWindowShouldClose(window)) {
		ProcessInput(window);

		// RENDER
		glClearColor(19 / 255.0, 167 / 255.0, 139 / 255.0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;

	return 0;
}