#pragma once

#include "sdk/VertexArray.hpp"
#include "sdk/Shader.hpp"
#include "sdk/Renderer.hpp"

class Planet
{
private:
	// some renderer stuff
	std::shared_ptr<VertexArray> _va;
	std::shared_ptr<Shader> _shader;

	// attribs
	glm::vec3 _pos;
	glm::vec3 _scale;
	glm::vec4 _color;
	float _mass;

	// update values
	float _degreeDelta{ 0.0f };
	float _lastFrame{ 0.0f };
	bool _isFirstEntry{ true };

public:
	Planet(std::shared_ptr<VertexArray>& va, std::shared_ptr<Shader>& shader, 
		float mass = 1.0f, glm::vec3 pos = {0.0f, 0.0f, 0.0f}, glm::vec3 scale = {1.0f, 1.0f, 1.0f}, glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f}) :
		_va(va), _shader(shader), _pos(pos), _scale(scale), _color(color), _mass(mass)
	{
	}

	Planet(const Planet& planet) :
		_va(planet._va), _shader(planet._shader), _pos(planet._pos), _scale(planet._scale), _color(planet._color), _mass(planet._mass)
	{

	}

	void draw(GLenum mode = GL_FILL);

	void update(const glm::vec3 center, const float centerMass, const float eccentricity, const float focalDistance);

	void scale(const glm::vec3 scale);

	void moveTo(const glm::vec3 pos);

	void setColor(const glm::vec4 color) { _color = color; };

	void setMass(const float mass) { _mass = mass; };

	const glm::vec4 color() const { return _color; };

	const glm::vec3 position() const { return _pos; };

	const float mass() const { return _mass; };
};

void Planet::update(const glm::vec3 center, const float centerMass, const float eccentricity, const float focalDistance)
{
	float distance = glm::length(_pos - center);
	if (distance == 0.f) return;
	float centralForce = 9999 * centerMass * _mass / (distance * distance);
	float angularVelocity = sqrtf(centralForce / (_mass * distance));

	if (_isFirstEntry)
	{
		_lastFrame = glfwGetTime();
		_isFirstEntry = false;
	}

	float currentFrame = (float)glfwGetTime();
	_degreeDelta += (currentFrame - _lastFrame) * angularVelocity;
	_lastFrame = currentFrame;

	float e = eccentricity;
	CLAMP(e, 0.01f, 0.99f);
	float ratio = sqrtf(1 - e * e);
	_pos.x = center.x + cosf(glm::radians(_degreeDelta)) * focalDistance;
	_pos.y = center.y;
	_pos.z = center.z + ratio * sinf(glm::radians(_degreeDelta)) * focalDistance;
}

void Planet::draw(GLenum mode)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), _pos);
	model = glm::scale(model, _scale);
	_shader->uniformMatrix4fv("u_model", model);
	_shader->uniform4fv("u_color", _color);
	_shader->uniform1i("u_shouldEnableLighting", 1);
	Renderer::getInstance()->draw(*_va, *_shader, mode, GL_TRIANGLES);
}

void Planet::scale(const glm::vec3 scale)
{
	_scale = scale;
}

void Planet::moveTo(const glm::vec3 pos)
{
	_pos = pos;
}

