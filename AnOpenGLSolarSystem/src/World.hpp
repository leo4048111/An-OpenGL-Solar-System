#pragma once

#include "Planet.hpp"
#include "Helper.hpp"
#include "sdk/Shader.hpp"

#include <queue>

typedef struct
{
	Planet* planet;
	std::string name;
	std::string centerPlanet;
	float eccentricity;
	float focalDistance;
	VertexArray* trail;
}PlanetInfo;

class World
{
	NONCOPYABLE(World)

public:
	World() = default;
	~World();

	void init(int horizontalLevel, int verticalLevel, float radius);

	void addPlanet(std::string name, std::string centerPlanet, float eccentricity, float focalDistance, std::shared_ptr<Shader>& shader,
		float mass = 1.0f, glm::vec3 pos = { 0.0f, 0.0f, 0.0f }, glm::vec3 scale = { 1.0f, 1.0f, 1.0f }, glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f });

	void draw(GLenum mode = GL_FILL);

	void onImGuiRender();

	// some additional utils

	void renderPlanetInfo(Camera& camera);

	void renderPlanetNames(Camera& camera, int displayW, int displayH);

	void renderStars(Camera& camera, std::shared_ptr<Shader>& shader, int count);

	void showTrails(Camera& camera, std::shared_ptr<Shader>& shader);

private:
	std::shared_ptr<VertexArray> _vaSphere;

	std::unordered_map<std::string, Planet*> _planetNameMap;

	std::vector<PlanetInfo> _planetInfos;

	std::vector<Planet> _stars;

	std::vector<VertexArray> _trails;
};

inline auto g_world = std::make_unique<World>();

void World::init(int horizontalLevel, int verticalLevel, float radius)
{
	_vaSphere = Helper::makeSphereVertexArray(horizontalLevel, verticalLevel, radius);
}

void World::addPlanet(std::string name, std::string centerPlanet, float eccentricity, float focalDistance, std::shared_ptr<Shader>& shader,
	float mass, glm::vec3 pos, glm::vec3 scale, glm::vec4 color)
{
	glm::vec3 center = { 0.0f, 0.0f, 0.0f };
	if (_planetNameMap.find(centerPlanet) != _planetNameMap.end()) center = _planetNameMap[centerPlanet]->position();
	Planet* planet = new Planet(_vaSphere, shader, mass, pos + center, scale, color);
	_planetNameMap.insert(std::make_pair(name, planet));

	// init trail vao
	std::vector<float> vertexes;
	std::vector<unsigned int> indices;
	float ratio = sqrtf(1 - eccentricity * eccentricity);
	float degree = 0.f;
	glm::vec3 trailPos;
	for (degree; degree < 360.f; degree += 0.5f)
	{
		trailPos.x = 0.f + cosf(glm::radians(degree)) * focalDistance;
		trailPos.y = 0.f;
		trailPos.z = 0.f + ratio * sinf(glm::radians(degree)) * focalDistance;
		vertexes.push_back(trailPos.x);
		vertexes.push_back(trailPos.y);
		vertexes.push_back(trailPos.z);
		vertexes.push_back(trailPos.x);
		vertexes.push_back(trailPos.y);
		vertexes.push_back(trailPos.z);
		indices.push_back(vertexes.size() - 1);
		indices.push_back(vertexes.size());
	}
	vertexes.push_back(trailPos.x);
	vertexes.push_back(trailPos.y);
	vertexes.push_back(trailPos.z);

	VertexBuffer vb(&vertexes[0], vertexes.size() * sizeof(float));
	IndexBuffer ib(&indices[0], indices.size());

	BufferLayout layout;
	layout.push(GL_FLOAT, 3, GL_FALSE);
	layout.push(GL_FLOAT, 3, GL_FALSE);

	VertexArray* va = new VertexArray(vb, ib, layout);

	_planetInfos.push_back({ planet, name, centerPlanet, eccentricity, focalDistance, va });
}

void World::draw(GLenum mode)
{
	for (auto& info : _planetInfos)
	{
		auto& planet = info.planet;
		if (_planetNameMap.find(info.centerPlanet) != _planetNameMap.end())
		{
			auto& centerPlanet = _planetNameMap[info.centerPlanet];
			planet->update(centerPlanet->position(), centerPlanet->mass(), info.eccentricity, info.focalDistance);
		}
	}

	for (auto& info : _planetInfos) info.planet->draw(mode);
}

void World::showTrails(Camera& camera, std::shared_ptr<Shader>& shader)
{
	shader->uniform1i("u_shouldEnableLighting", 0);
	for (auto& info : _planetInfos)
	{
		auto& centerPlanet = _planetNameMap[info.centerPlanet];
		glm::mat4 model = glm::translate(glm::mat4(1.0f), centerPlanet->position());
		shader->uniformMatrix4fv("u_model", model);
		shader->uniform4fv("u_color", info.planet->color());
		Renderer::getInstance()->draw(*info.trail, *shader, GL_FILL, GL_LINES);
	}
}

void World::renderPlanetInfo(Camera& camera)
{
	float indent = 0;
	char buf1[256];
	sprintf_s(buf1, "Camera position: x: %.3f, y: %.3f, z: %.3f, pitch: %.3f, yaw: %.3f", camera.position().x, camera.position().y, camera.position().z
		, camera.pitch(), camera.yaw());
	ImGui::GetBackgroundDrawList()->AddText({ 20, indent }, ImGui::ColorConvertFloat4ToU32({ 255.f, 255.f, 255.f, 255.f }), buf1);
	for (auto& info : _planetInfos)
	{
		char buf[256];
		sprintf_s(buf, "%s: x: %.3lf, y: %.3lf, z: %.3lf, mass, %.3lf, center: %s, ecc: %.3lf, fd: %.3lf", info.name.c_str(),
			info.planet->position().x, info.planet->position().y, info.planet->position().z, info.planet->mass(), info.centerPlanet.c_str(), info.eccentricity, info.focalDistance);
		ImGui::GetBackgroundDrawList()->AddText({ 20, indent + 20 }, ImGui::ColorConvertFloat4ToU32({ 255.f, 255.f, 255.f, 255.f }), buf);
		indent += 20;
	}
}

void World::renderPlanetNames(Camera& camera, int displayW, int displayH)
{
	for (auto& info : _planetInfos)
	{
		glm::vec4 clipCoord = camera.projectionMatrix() * camera.viewMatrix() * glm::vec4(info.planet->position(), 1.0f);
		if (clipCoord.w >= 0.1f) {
			glm::vec3 ndc = { clipCoord.x / clipCoord.w, clipCoord.y / clipCoord.w, clipCoord.z / clipCoord.w };
			glm::vec2 screenCoords(displayW / 2 * ndc.x + ndc.x + displayW / 2, -displayH / 2 * ndc.y + ndc.y + displayH / 2);
			ImGui::GetBackgroundDrawList()->AddText({ screenCoords.x, screenCoords.y }, ImGui::ColorConvertFloat4ToU32({ 255.f, 255.f, 255.f, 255.f }), info.name.c_str());
		}
	}
}

void World::renderStars(Camera& camera, std::shared_ptr<Shader>& shader, int count)
{
	if (_stars.size() > count) _stars.clear();
	while (_stars.size() < count)
	{
		glm::vec3 pos;
		pos.x = glm::linearRand(camera.position().x - 400.f, camera.position().x + 400.f);
		pos.y = glm::linearRand(camera.position().y - 400.f, camera.position().y + 400.f);
		pos.z = glm::linearRand(camera.position().z - 400.f, camera.position().z + 400.f);
		Planet planet(_vaSphere, shader, 0.0f, pos, { 0.01f, 0.01f, 0.01f },  {1.0f, 1.0f, 1.0f, 1.0f});
		_stars.push_back(planet);
	}

	std::vector<int> removeList;

	for (auto& star : _stars)
	{
		if (glm::length(star.position() - camera.position()) < glm::length(glm::vec3(200.f, 200.f, 200.f)))
		{
			glm::vec3 pos;
			pos.x = glm::linearRand(camera.position().x - 400.f, camera.position().x + 400.f);
			pos.y = glm::linearRand(camera.position().y - 400.f, camera.position().y + 400.f);
			pos.z = glm::linearRand(camera.position().z - 400.f, camera.position().z + 400.f);
			star.moveTo(pos);
		}
	}


	for (auto& star : _stars) star.draw();
}

void World::onImGuiRender()
{
	ImGui::BeginChild("#planet edit", { 0,0 }, true);
	for (auto& info : _planetInfos)
	{
		ImGui::Text("%s: ", info.name.c_str()); ImGui::SameLine();
		ImGui::PushItemWidth(75.2);

		char id1[64];
		sprintf_s(id1, "Eccentricity##%s", info.name.c_str());
		char id2[64];
		sprintf_s(id2, "FocalDistance##%s", info.name.c_str());
		char id3[64];
		sprintf_s(id3, "Mass##%s", info.name.c_str());
		char id4[64];
		sprintf_s(id4, "Planet color##%s", info.name.c_str());

		ImGui::SliderFloat(id1, &info.eccentricity, 0.01f, 0.99f, "%.2lf"); ImGui::SameLine();
		ImGui::SliderFloat(id2, &info.focalDistance, 0.f, 1000.f, "%.2lf"); ImGui::SameLine();
		float mass = info.planet->mass();
		if (ImGui::SliderFloat(id3, &mass, 1.f, 999999.f, "%.2lf")) info.planet->setMass(mass);
		ImGui::PopItemWidth();
		glm::vec4 color = info.planet->color();
		if (ImGui::ColorEdit4(id4, &color.x)) info.planet->setColor(color);
	}
}

World::~World()
{
	for (auto& info : _planetInfos)
	{
		delete info.planet;
		delete info.trail;
	}
}


