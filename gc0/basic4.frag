#version 330 core

in vec3 fPos;
in vec2 fUV;
out vec4 colorResult;


void main() {
  colorResult = vec4(fUV, 0.0, 1.0);
}