#version 330 core

layout(location = 0) out vec4 FragColor;
uniform vec3 u_Color = vec3(1.0f, 0.f, 0.f);

void main()
{
	FragColor = vec4(u_Color, 1.0f);
};