#include "stdafx.h"
#include "stb_image.h"

#include "Shader.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

extern float vertices[];
extern int vertices_size;
extern glm::vec3 cube_positions[];
extern int cube_positions_size;

int g_width = 800;
int g_height = 600;

#pragma region Camera
glm::vec3 g_camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 g_camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 g_camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
float g_camera_speed_a = 2.5f;
float g_yaw = -90.0f;  // Æ«º½
float g_pitch = 0.0f;  // ¸©Ñö
float g_fov = 45.0f;
float g_ratio = g_width / static_cast<float>(g_height);
float g_near_plane = 0.1f;
float g_far_plane = 100.0f;
#pragma endregion

#pragma region Time
float g_delta = 0.0f;
float g_last_frame = 0.0f;
#pragma endregion

#pragma region Mouse
bool g_cursor_capturing = true;
float g_last_x = 400;
float g_last_y = 300;
float g_sensitivity = 0.1f;
bool g_first_move = true;
#pragma endregion

template <typename T>
T Clamp(T v, T min, T max) {
	if (v < min) {
		return min;
	}
	if (v > max) {
		return max;
	}
	return v;
}

void OnWindowResize(GLFWwindow* window, int width, int height) {
	spdlog::info("current window size: {}x{}\n", width, height);
	g_width = width;
	g_height = height;
	glViewport(0, 0, width, height);
}

void OnMouseMove(GLFWwindow* window, double xpos_raw, double ypos_raw) {
	if (g_cursor_capturing) {
		float xpos = static_cast<float>(xpos_raw);
		float ypos = static_cast<float>(ypos_raw);
		if (g_first_move) {
			g_last_x = xpos;
			g_last_y = ypos;
			g_first_move = false;
		}
		float xoffset = xpos - g_last_x;
		float yoffset = g_last_y - ypos;
		g_last_x = xpos;
		g_last_y = ypos;

		xoffset *= g_sensitivity;
		yoffset *= g_sensitivity;

		g_yaw += xoffset;
		g_pitch = Clamp(g_pitch + yoffset, -89.0f, 89.0f);

		glm::vec3 front(1.0f);
		front.x = cos(glm::radians(g_yaw)) * cos(glm::radians(g_pitch));
		front.y = sin(glm::radians(g_pitch));
		front.z = cos(glm::radians(g_pitch)) * sin(glm::radians(g_yaw));
		g_camera_front = glm::normalize(front);
	}
}
void OnMouseScroll(GLFWwindow* window, double xoffset_raw, double yoffset_raw) {
	if (g_cursor_capturing) {
		float yoffset = static_cast<float>(yoffset_raw);
		g_fov = Clamp(g_fov - yoffset, 1.0f, 180.0f);
	}
}

void ProcessInput(GLFWwindow* window) {
	glfwPollEvents();
	float camera_speed = g_camera_speed_a * g_delta;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		g_camera_pos += camera_speed * g_camera_front;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		g_camera_pos -= camera_speed * g_camera_front;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		g_camera_pos -= glm::normalize(glm::cross(g_camera_front, g_camera_up)) * camera_speed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		g_camera_pos += glm::normalize(glm::cross(g_camera_front, g_camera_up)) * camera_speed;
	}
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
	glfwSetCursorPosCallback(window, OnMouseMove);
	glfwSetScrollCallback(window, OnMouseScroll);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

#pragma region Loop Begin
	while (!glfwWindowShouldClose(window)) {
		float current = glfwGetTime();
		g_delta = current - g_last_frame;
		g_last_frame = current;

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
		ImGui::SliderFloat("mouse sensitivity", &g_sensitivity, 0.05f, 1.0f);
		ImGui::SliderFloat("move speed", &g_camera_speed_a, 1.0f, 10.0f);

		ImGui::SliderFloat("fov", &g_fov, 15.0f, 180.0f);
		ImGui::SliderFloat("ratio", &g_ratio, 0.5f, 5.0f);
		ImGui::SliderFloat("near plane", &g_near_plane, 0.01f, 10.0f);
		ImGui::SliderFloat("far plane", &g_far_plane, 10.0f, 500.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(g_fov), g_ratio, g_near_plane, g_far_plane);
		s.setUniform("projection", projection);

		glm::mat4 view(1.0f);
		view = glm::lookAt(g_camera_pos, g_camera_pos + g_camera_front, g_camera_up);
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