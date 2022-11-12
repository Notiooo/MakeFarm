#version 330 core

layout(location = 0) out vec4 FragColor;

void main()
{
    vec3 color = vec3(1.0f, 0.f, 0.f);
	FragColor = vec4(color, 1.0f);
};