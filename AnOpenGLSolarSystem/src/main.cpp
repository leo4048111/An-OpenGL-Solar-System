#include "sdk/Camera.hpp"
#include "sdk/Controller.hpp"
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include "World.hpp"

const int windowWidth = 1280, windowHeight = 720;
bool g_showMenu = true;

static bool init(GLFWwindow*& window)
{
	if (!glfwInit())
	{
		return false;
	}

	window = glfwCreateWindow(windowWidth, windowHeight, "Solar System", nullptr, nullptr);

	if (window == nullptr)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		return false;

	glEnable(GL_DEPTH_TEST);

	return true;
}

int main()
{
	GLFWwindow* window = nullptr;
	if (!init(window)) return 0;

	// init camera
	glm::mat4 projection(1.0f);
	projection = glm::perspective(glm::radians(45.f), (float)windowWidth / windowHeight, 0.1f, 1000.f);
	Camera camera({ -165.291, 233.284, 360.599 }, projection, -30.750, -69.750);

	// init shader
	auto shader = std::make_shared<Shader>("src/shaders/shader.vert", "src/shaders/shader.frag");
	shader->uniformMatrix4fv("u_projection", camera.projectionMatrix());

	// install controller
	Controller::getInstance()->install(window, &camera);

	// init imgui
	ImGui::CreateContext();
	ImGui_ImplOpenGL3_Init();
	ImGui_ImplGlfw_InitForOpenGL(window, GL_TRUE);
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.WantSaveIniSettings = false;

	// attribs of 8 planets + sun + moon
	float sunMass = 333400.f, mercuryMass = 1.f, venusMass = 1.f, earthMass = 4000.f, moonMass = 1.f, marsMass = 1.f, jupiterMass = 3000.f, saturnMass = 3000.f, uranusMass = 1.f, neptuneMass = 1.f, plutoMass = 1.f;
	float mercuryE = 0.6f, venusE = 0.65f, earthE = 0.7f, moonE = 0.7f, marsE = 0.72f, jupiterE = 0.75f, saturnE = 0.79f, uranusE = 0.81f, neptuneE = 0.83f, plutoE = 0.85f;
	float mercuryFD = 30.f, venusFD = 60.f, earthFD = 100.f, moonFD = 40.f, marsFD = 140.f, jupiterFD = 170.f, saturnFD = 200.f, uranusFD = 230.f, neptuneFD = 260.f, plutoFD = 300.f;
	glm::vec3 sunScale = { 1.0f, 1.0f, 1.0f }, mercuryScale = { 0.2f, 0.2f, 0.2f }, venusScale = { 0.3f, 0.3f, 0.3f }, earthScale = { 0.5f, 0.5f, 0.5f }, moonScale = { 0.1f, 0.1f, 0.1f };
	glm::vec3 marsScale = { 0.28f, 0.28f, 0.28f }, jupiterScale = { 0.7f, 0.7f, 0.7f }, saturnScale = { 0.65f, 0.65f, 0.65f }, uranusScale = { 0.38f, 0.38f, 0.38f }, neptuneScale = { 0.38f, 0.38f, 0.38f };
	glm::vec3 plutoScale = { 0.2f, 0.2f, 0.2f };
	glm::vec4 sunColor = { 1.0f, 0.0f, 0.0f, 1.0f }, mercuryColor = { 0.75f, 0.45f, 0.13f, 1.0f }, venusColor = {0.55f, 0.44f, 0.27f, 1.0f}, earthColor = { 0.0f, 0.0f, 1.0f, 1.0f }, moonColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec4 marsColor = { 0.73f, 0.33f, 0.23, 1.0f }, jupiterColor = { 0.57f, 0.40f, 0.25, 1.0f }, saturnColor = { 0.89f, 0.71f, 0.49f, 1.0f }, uranusColor = { 0.16f,0.75f, 0.93f, 1.0f }, neptuneColor = { 0.16f,0.75f, 0.93f, 1.0f };
	glm::vec4 plutoColor = { 0.46f, 0.67f, 0.71f, 1.0f };

	// init world
	g_world->init(50, 50, 20.f);
	g_world->addPlanet("Sun", "Sun", 1.0f, 0.0f, shader, sunMass, { 0.0f, 0.0f, 0.0f }, sunScale, sunColor);
	g_world->addPlanet("Mercury", "Sun", mercuryE, mercuryFD, shader, mercuryMass, { mercuryFD, 0.0f, 0.0f }, mercuryScale, mercuryColor);
	g_world->addPlanet("Venus", "Sun", venusE, venusFD, shader, venusMass, { venusFD, 0.0f, 0.0f }, venusScale, venusColor);
	g_world->addPlanet("Earth", "Sun", earthE, earthFD, shader, earthMass, { earthFD, 0.0f, 0.0f }, earthScale, earthColor);
	g_world->addPlanet("Mars", "Sun", marsE, marsFD, shader, marsMass, { marsFD, 0.0f, 0.0f }, marsScale, marsColor);
	g_world->addPlanet("Jupiter", "Sun", jupiterE, jupiterFD, shader, jupiterMass, { jupiterFD, 0.0f, 0.0f }, jupiterScale, jupiterColor);
	g_world->addPlanet("Saturn", "Sun", saturnE, saturnFD, shader, saturnMass, { saturnFD, 0.0f, 0.0f }, saturnScale, saturnColor);
	g_world->addPlanet("Uranus", "Sun", uranusE, uranusFD, shader, uranusMass, { uranusFD, 0.0f, 0.0f }, uranusScale, uranusColor);
	g_world->addPlanet("Neptune", "Sun", neptuneE, neptuneFD, shader, neptuneMass, { neptuneFD, 0.0f, 0.0f }, neptuneScale, neptuneColor);
	g_world->addPlanet("Pluto", "Sun", plutoE, plutoFD, shader, plutoMass, { plutoFD, 0.0f, 0.0f }, plutoScale, plutoColor);

	// some satellites just for fun
	g_world->addPlanet("Moon", "Earth", moonE, moonFD, shader, moonMass, { moonFD, 0.0f, 0.0f }, moonScale, moonColor);
	g_world->addPlanet("Ganymede", "Jupiter", 0.6, 50.f, shader, 1.0f, { moonFD, 0.0f, 0.0f }, moonScale, { 0.21f, 0.22 , 0.17 ,1.0f });
	g_world->addPlanet("Titan", "Saturn", 0.6, 50.f, shader, 1.0f, { moonFD, 0.0f, 0.0f }, moonScale, saturnColor);

	shader->uniform3fv("u_lightColor", {1.0f, 1.0f, 1.0f});
	shader->uniform3fv("u_lightPos", { 0.0f, 0.0f, 0.0f });

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// world render
		shader->uniformMatrix4fv("u_view", camera.viewMatrix());
		shader->uniform3fv("u_viewPos", camera.position());
		g_world->draw();

		// A better way of dealing with custom key binds is to implement addListener in Controller class(which i'll be doing later)
		static int lastInsState = 0;
		if (lastInsState != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_INSERT) == GLFW_PRESS) g_showMenu = !g_showMenu;
		lastInsState = glfwGetKey(window, GLFW_KEY_INSERT);

		// topmost menu
		static int display_w, display_h;
		static bool shouldRenderPlanetNames = true, shouldRenderBasicStats = true, shouldDrawStars = true, shouldShowTrails = true;
		static int starCnt = 3000;
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();
		if (shouldShowTrails) g_world->showTrails(camera, shader);
		if (shouldDrawStars) g_world->renderStars(camera, shader, starCnt);
		if (shouldRenderPlanetNames) g_world->renderPlanetNames(camera, display_w, display_h);
		if (shouldRenderBasicStats) g_world->renderPlanetInfo(camera);
		ImGui::GetForegroundDrawList()->AddText({ 20, (float)display_h - 30 }, ImGui::ColorConvertFloat4ToU32({ 255.f, 255.f, 255.f, 255.f }), "CS10043301 assignment2: A basic solar system made by 2050250.");
		if (g_showMenu)
		{
			Controller::getInstance()->pause();

			ImGui::SetNextWindowSize({ 570, 480 }, ImGuiCond_FirstUseEver);
			ImGui::Begin("Menu(Readme)", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
			ImGui::Text("Keybinds for this demo system(Controllers only work when menu is closed!):");
			ImGui::Text("Press [INSERT] to open/close menu");
			ImGui::Text("Hold [W][A][S][D] to move forward/left/backward/right");
			ImGui::Text("Hold [SPACE][CTRL] to move up/down");
			ImGui::Text("Move mouse to look around.");

			// controls
			ImGui::Checkbox("Render basic stats", &shouldRenderBasicStats); ImGui::SameLine();
			ImGui::Checkbox("Render planet names", &shouldRenderPlanetNames); ImGui::SameLine();
			ImGui::Checkbox("Show trails", &shouldShowTrails); ImGui::SameLine();
			ImGui::Checkbox("Galaxy skybox", &shouldDrawStars);
			if(shouldDrawStars)
			ImGui::SliderInt("Star count", &starCnt, 1000, 5000);
			g_world->onImGuiRender();
			ImGui::End();
		}
		else Controller::getInstance()->resume();
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	Controller::getInstance()->uninstall();

	//glfwTerminate();
	return 0;
}
