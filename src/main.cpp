#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "shader.h"
#include "load.h"
#include "camera.h"
#include <iostream>

//This will be a Volume Renderer that is able to view .raw medical data 
//Should be able to view CT scans of different things
//5/1 start - implement libs/start project with window + game loop + shaders!
//https://github.com/wilsonCernWq/open-scivis-datasets
//TODO: look into DICOM parsing or .raw conversion?
int main()
{
	unsigned int SCR_WIDTH = 800;
	unsigned int SCR_HEIGHT = 600;
	bool running = true;
	

	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


	SDL_Window* window = SDL_CreateWindow("GL Volume Viewer - Medical", 
										   SCR_WIDTH, SCR_HEIGHT, 
										   SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!window)
	{
		std::cout << "Failed to Create Window \n";
		SDL_Quit();
		return -1;
	}


	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (!context)
	{
		std::cout << "Failed to Create a GL Context\n";
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	SDL_GL_MakeCurrent(window, context);

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::cout << "Failed to load GLAD\n";
		SDL_GL_DestroyContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO* IO = &ImGui::GetIO();
	IO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL3_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init("#version 460");


	float cubeVertices[] = {
		// back face
		-1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 // front face
		 -1.0f, -1.0f,  1.0f,
		  1.0f, -1.0f,  1.0f,
		  1.0f,  1.0f,  1.0f,
		 -1.0f, -1.0f,  1.0f,
		  1.0f,  1.0f,  1.0f,
		 -1.0f,  1.0f,  1.0f,
		 // left face
		 -1.0f,  1.0f,  1.0f,
		 -1.0f,  1.0f, -1.0f,
		 -1.0f, -1.0f, -1.0f,
		 -1.0f, -1.0f, -1.0f,
		 -1.0f, -1.0f,  1.0f,
		 -1.0f,  1.0f,  1.0f,
		 // right face
		  1.0f,  1.0f,  1.0f,
		  1.0f, -1.0f, -1.0f,
		  1.0f,  1.0f, -1.0f,
		  1.0f, -1.0f, -1.0f,
		  1.0f,  1.0f,  1.0f,
		  1.0f, -1.0f,  1.0f,
		  // bottom face
		  -1.0f, -1.0f, -1.0f,
		   1.0f, -1.0f, -1.0f,
		   1.0f, -1.0f,  1.0f,
		  -1.0f, -1.0f, -1.0f,
		   1.0f, -1.0f,  1.0f,
		  -1.0f, -1.0f,  1.0f,
		  // top face
		  -1.0f,  1.0f, -1.0f,
		   1.0f,  1.0f,  1.0f,
		   1.0f,  1.0f, -1.0f,
		  -1.0f,  1.0f, -1.0f,
		  -1.0f,  1.0f,  1.0f,
		   1.0f,  1.0f,  1.0f
	};

	//Setup
	Camera camera;
	Shader mainShader("shaders/shader.vert", "shaders/shader.frag");

	//Cube for our volume rendering
	uint32_t cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	//load raw volume data
	const std::string FILE_PATH = "scans/foot_256x256x256_uint8.raw";
	const int WIDTH = 256;
	const int HEIGHT = 256;
	const int DEPTH = 256;
	std::vector<unsigned char> volumeData;
	if (!LoadVolume(FILE_PATH, WIDTH, HEIGHT, DEPTH, volumeData)) 
	{
		return 1; 
	}

	//Create a 3D texture that will store our volume data so we can send it to gpu
	uint32_t volumeTex;
	glGenTextures(1, &volumeTex);
	glBindTexture(GL_TEXTURE_3D, volumeTex);
	//tex wrapping
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//tex filtering	
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//data
	//(1 red channel, 8-bit)
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, WIDTH, HEIGHT, DEPTH, 0, GL_RED, GL_UNSIGNED_BYTE, volumeData.data());
	
	glEnable(GL_DEPTH_TEST);
	
	//imgui controls
	int stepCount = 128;




	SDL_Event e;
	while (running)
	{
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{

			case SDL_EVENT_QUIT:
				running = false;
				break;

			case SDL_EVENT_KEY_DOWN:

				switch (e.key.key)
				{
				case SDLK_ESCAPE:
					running = false;
					break;
				}
			case SDL_EVENT_MOUSE_MOTION:
				if (!IO->WantCaptureMouse && (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_MASK(SDL_BUTTON_LEFT))) 
				{
					float xoffset = static_cast<float>(e.motion.xrel);
					float yoffset = static_cast<float>(-e.motion.yrel);
					camera.ProcessMouseMovement(xoffset, yoffset);
				}
				break;
			case SDL_EVENT_MOUSE_WHEEL:
				if (!IO->WantCaptureMouse)
				{
					float scrollY = static_cast<float>(e.wheel.y);
					camera.ProcessMouseScroll(scrollY);
				}
				break;
			}
			ImGui_ImplSDL3_ProcessEvent(&e);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.18f, 0.18f, 0.18f, 1.0f);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();



		mainShader.use();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		mainShader.setMat4("projection", projection);
		mainShader.setMat4("view", view);

		mainShader.setVec3("cameraPos", camera.GetPosition());

		glm::mat4 modelMtx = glm::mat4(1.0f);
		mainShader.setMat4("model", modelMtx);

		//DRAW
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, volumeTex);
		mainShader.setInt("volumeTex", 0);
		mainShader.setInt("maxSteps", stepCount);
		mainShader.setFloat("alphaScale", 0.5f);
		mainShader.setVec3("cameraPos", camera.GetPosition());
		mainShader.setVec3("lightDir", glm::normalize(glm::vec3(1, 1, 1)));
		mainShader.setVec3("lightColor", glm::vec3(1.0f));
		mainShader.setFloat("shininess", 32.0f);
		mainShader.setFloat("specStrength", 0.5f);

		glDrawArrays(GL_TRIANGLES, 0, 36);




		ImGui::Begin("Volume Rendering - Debug");
		ImGui::SliderInt("StepCount", &stepCount, 128, 1024);
		ImGui::End();
		

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}


	glDeleteTextures(1, &volumeTex);

	return 0;
}