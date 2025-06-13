#version 330 core

in vec3 v_normal;
in vec2 v_texCoord;
in vec3 v_fragPosition;

uniform vec3 u_lightPosition;
uniform vec3 u_lightColor;
uniform vec3 u_objectColor;
uniform float u_ambientStrength;

uniform float u_specularStrength;
uniform float u_specularShiness;
uniform vec3 u_viewPosition;

out vec4 fragColor;

void main() {
  vec3 ambient = u_ambientStrength * u_lightColor;

  vec3 lightDir = normalize(u_lightPosition - v_fragPosition);
  vec3 fragNormal = normalize(v_normal);
  vec3 diffuse = max(dot(fragNormal, lightDir), 0.0) * u_lightColor;

  vec3 viewDir = normalize(u_viewPosition - v_fragPosition);
  vec3 reflectDir = reflect(-lightDir, fragNormal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_specularShiness);
  vec3 specular = u_specularStrength * spec * u_lightColor;

  vec3 finalColor = (ambient + diffuse + specular) * u_objectColor;
  fragColor = vec4(finalColor, 1.0);
}