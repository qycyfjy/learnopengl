#include "stdafx.h"
#include "stb_image.h"

#include "Shader.h"

extern float vertices[];
extern int vertices_size;
extern glm::vec3 cube_positions[];
extern int cube_positions_size;

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

int g_width = 800;
int g_height = 600;

void OnWindowResize(GLFWwindow* window, int width, int height) {
	spdlog::info("current window size: {}x{}\n", width, height);
	g_width = width;
	g_height = height;
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window) {
	glfwPollEvents();
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

int main() {
#pragma region Some Init
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto window = glfwCreateWindow(g_width, g_height, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr) {
		printf("failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, OnWindowResize);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("failed to init GLAD\n");
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, g_width, g_height);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	stbi_set_flip_vertically_on_load(true);
#pragma endregion

#pragma region Vertex, Texture, etc...
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int w, h, nc;
	unsigned char* data = stbi_load("rc/b.jpg", &w, &h, &nc, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		spdlog::error("failed to load texture");
	}
	stbi_image_free(data);
#pragma endregion

	Shader s("rc/vertex.txt", "rc/fragment.txt");
	s.use();
	s.setUniform("texture1", 0);

	float fov = 45.0f;
	float ratio = g_width / static_cast<float>(g_height);
	float near_plane = 0.1f;
	float far_plane = 100.0f;

#pragma region Loop Begin
	while (!glfwWindowShouldClose(window)) {
		ProcessInput(window);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glClearColor(19 / 255.0f, 167 / 255.0f, 139 / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui::Text("%.1f FPS", io.Framerate);
#pragma endregion

		///
		/// Ö÷Âß¼­
		/// 
		ImGui::SliderFloat("fov", &fov, 15.0f, 180.0f);
		ImGui::SliderFloat("ratio", &ratio, 0.5f, 5.0f);
		ImGui::SliderFloat("near plane", &near_plane, 0.01f, 10.0f);
		ImGui::SliderFloat("far plane", &far_plane, 10.0f, 500.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(fov), ratio, near_plane, far_plane);
		s.setUniform("projection", projection);

		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		s.setUniform("view", view);
		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model(1.0f);
			model = glm::translate(model, cube_positions[i]);
			float angle = 20.0f * i + 10;
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			s.setUniform("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
#pragma region Loop End
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
#pragma endregion

#pragma region Some Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
#pragma endregion

	return 0;
}