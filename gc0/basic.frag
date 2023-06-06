#version 330 core

in vec3 fPos;
in vec3 fNormal;

out vec4 colorResult;

uniform vec3 figureColor;


void main() {
  colorResult = vec4(figureColor, 1.0); 
}