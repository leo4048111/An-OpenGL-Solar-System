#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 o_normal;
out vec3 o_fragPos;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(pos, 1.0);
    o_normal = mat3(transpose(inverse(u_model))) * normal;
    o_fragPos = vec3(u_model * vec4(pos, 1.0));
}