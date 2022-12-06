#version 330 core

layout(location = 0) in vec3  position;
layout(location = 1) in vec2  texCoord;
layout(location = 2) in float directionalLightning;

out vec2 v_TexCoord;
out float v_DirectionalLightning;

uniform mat4 u_ViewProjection;

void main()
{
	gl_Position = u_ViewProjection * vec4(position, 1.0);
	v_TexCoord = texCoord;
	v_DirectionalLightning = directionalLightning;
};