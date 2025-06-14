#version 330 core

uniform vec3 u_lightColor;
uniform float u_brightness;

out vec4 fragColor;

void main() {
  fragColor = vec4(u_lightColor * u_brightness, 1.0);
}