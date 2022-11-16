#include "stdafx.h"
#include "stb_image.h"

#include "Shader.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

void OnWindowResize(GLFWwindow* window, int width, int height) {
	spdlog::info("current window size: {}x{}\n", width, height);
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

	auto window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
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
	glViewport(0, 0, 800, 600);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	stbi_set_flip_vertically_on_load(true);
#pragma endregion

	float vertices[] = {
		0.5f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f,    // 右上
		0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,    // 右下
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
	};

	unsigned int indices[] = {
		0,1,2,
		0,3,2,
	};

#pragma region Vertex, Texture, etc...
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	unsigned int VEO;
	glGenBuffers(1, &VEO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VEO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	int w, h, nc;
	unsigned char* data = stbi_load("rc/a.jpg", &w, &h, &nc, 0);
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	data = stbi_load("rc/b.jpg", &w, &h, &nc, 0);
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
#pragma endregion

	Shader s("rc/vertex.txt", "rc/fragment.txt");
	s.use();
	s.setUniform("texture1", 0);
	s.setUniform("texture2", 1);

	float mix = 0.0f;
	float scale = 0.5;
	float rotate = 90.0;
	bool auto_rotate = false;
	float auto_rotate_speed = 90.0f;
	float move_v = 0.0f;
	float move_h = 0.0f;

#pragma region Loop Begin
	while (!glfwWindowShouldClose(window)) {
		ProcessInput(window);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
#pragma endregion

		///
		/// 主逻辑
		/// 
		ImGui::SliderFloat("mixv", &mix, 0.0f, 1.0f);
		s.setUniform("mixv", mix);

		// 缩放
		glm::mat4 trans(1.0f);
		ImGui::SliderFloat("scale", &scale, 0.0f, 10.0f);
		trans = glm::scale(trans, glm::vec3(scale, scale, scale));

		// 先位移
		ImGui::SliderFloat("Move(V)", &move_v, -3.0f, 3.0f);
		ImGui::SliderFloat("Move(H)", &move_h, -3.0f, 3.0f);
		trans = glm::translate(trans, glm::vec3(move_v, move_h, 1.0f));

		// 后旋转
		ImGui::Checkbox("Auto Rotate", &auto_rotate);
		if (auto_rotate) {
			ImGui::SliderFloat("Speed", &auto_rotate_speed, 1.0f, 7200.0f);
			rotate = fmodf(rotate + ImGui::GetIO().DeltaTime * auto_rotate_speed, 360.0f);
		}
		else {
			ImGui::SliderFloat("rotate", &rotate, 0.0f, 360.0f);
		}
		trans = glm::rotate(trans, glm::radians(rotate), glm::vec3(0.0, 0.0, 1.0));

		s.setUniform("transform", trans);

		// RENDER
		glClearColor(19 / 255.0f, 167 / 255.0f, 139 / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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