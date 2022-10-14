#pragma once

#include "sdk/Headers.hpp"
#include "sdk/VertexArray.hpp"

class Helper
{
	INCONSTRUCTIBLE(Helper)

public:
	static std::shared_ptr<VertexArray> makeSphereVertexArray(int horizontalLevel, int verticalLevel, float radius);
};

// make vertex array which represents a sphere
std::shared_ptr<VertexArray> Helper::makeSphereVertexArray(int horizontalLevel, int verticalLevel, float radius)
{
	std::vector<float> coords;
	std::vector<unsigned int> indices;
	float pitchMod = 180.f / verticalLevel;
	float yawMod = 360.f / horizontalLevel;

	// topmost point
	coords.push_back(0.0f);
	coords.push_back(radius);
	coords.push_back(0.0f);

	for (int i = 1; i < verticalLevel; i++)
	{
		for (int j = 0; j < horizontalLevel; j++)
		{
			float yaw = j * yawMod;
			float pitch = i * pitchMod - 90.f;
			float x = radius * cosf(glm::radians(pitch)) * cosf(glm::radians(yaw));
			float y = -radius * sinf(glm::radians(pitch));
			float z = radius * cosf(glm::radians(pitch)) * sinf(glm::radians(yaw));
			coords.push_back(x);
			coords.push_back(y);
			coords.push_back(z);
			
			// normal vector
			coords.push_back(x - 0);
			coords.push_back(y - 0);
			coords.push_back(z - 0);

			if (i == 1)
			{
				indices.push_back(j % horizontalLevel + 1);
				indices.push_back((j + 1) % horizontalLevel + 1);
				indices.push_back(0);
			}

			if (i != verticalLevel - 1)
			{
				indices.push_back((i - 1) * horizontalLevel + j % horizontalLevel + 1);
				indices.push_back((i - 1) * horizontalLevel + (j + 1) % horizontalLevel + 1);
				indices.push_back(i * horizontalLevel + j % horizontalLevel + 1);

				indices.push_back(i * horizontalLevel + j % horizontalLevel + 1);
				indices.push_back(i * horizontalLevel + (j + 1) % horizontalLevel + 1);
				indices.push_back((i - 1) * horizontalLevel + (j + 1) % horizontalLevel + 1);
			}
			else
			{
				indices.push_back((i - 1) * horizontalLevel + j % horizontalLevel + 1);
				indices.push_back((i - 1) * horizontalLevel + (j + 1) % horizontalLevel + 1);
				indices.push_back(1 + (verticalLevel - 1) * horizontalLevel);
			}
		}
	}

	// bottom most point
	coords.push_back(0.0f);
	coords.push_back(-radius);
	coords.push_back(0.0f);

	VertexBuffer vb(&coords[0], coords.size() * sizeof(float));
	IndexBuffer ib(&indices[0], indices.size());

	BufferLayout layout;
	layout.push(GL_FLOAT, 3, GL_FALSE);
	layout.push(GL_FLOAT, 3, GL_FALSE);

	return std::make_shared<VertexArray>(vb, ib, layout);
}

