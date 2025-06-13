#version 330 core

layout (location = 0) in vec3 a_position;  // a_ : attribute
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoord;

uniform mat4 u_transform;  // u_ : uniform
uniform mat4 u_modelTransform;

out vec3 v_normal;  // v_ : varying
out vec2 v_texCoord;
out vec3 v_fragPosition;

void main() {
  gl_Position = u_transform * vec4(a_position, 1.0);
  v_normal = (transpose(inverse(u_modelTransform)) * vec4(a_normal, 0.0)).xyz;
  v_texCoord = a_texCoord;
  v_fragPosition = (u_modelTransform * vec4(a_position, 1.0)).xyz;
}