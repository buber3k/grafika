#version 330 core

in vec3 fPos;
out vec4 colorResult;

uniform vec3 ColorTriangle;

void main() {
  colorResult = vec4(ColorTriangle, 1.0);
}