#version 330 core

in vec3 fPos;
in vec3 fNormal;
in vec2 fUV;

out vec4 colorResult;

uniform sampler2D fTexture;

uniform float intensity;

void main() {
  colorResult = mix(texture(fTexture, fUV), vec4(0.3, 0.5, 0.7, 1.0), intensity); 
}