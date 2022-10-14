#version 330 core

out vec4 color;
in vec3 o_normal;
in vec3 o_fragPos;

uniform vec4 u_color;
uniform vec3 u_lightColor;
uniform vec3 u_lightPos;
uniform vec3 u_viewPos;

uniform int u_shouldEnableLighting;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

void main()
{
	if(u_shouldEnableLighting == 0) 
	{
		color = u_color;
		return;
	}

	Material material;
	material.ambient = vec3(1.0, 0.5, 0.31);
	material.diffuse = vec3(1.0, 0.5, 0.31);
	material.specular = vec3(0.5, 0.5, 0.5);
	material.shininess = 32.0;

	// ambient light
	vec3 ambient = u_lightColor * material.ambient;

	// diffuse light
	vec3 norm = normalize(o_normal);
	vec3 lightDir = normalize(u_lightPos - o_fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = u_lightColor * (diff * material.diffuse);

	// specular light
	vec3 viewDir = normalize(u_viewPos - o_fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = u_lightColor * (spec * material.specular);

	vec3 result = (ambient + diffuse + specular) * u_color.xyz;
	color = vec4(result, 1.0);
}