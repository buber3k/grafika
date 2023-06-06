#version 330 core

layout(location = 0) in vec3 iPos;
layout(location = 1) in vec2 iUV;

out vec3 fPos;

uniform mat4 Model;

void main() {
  gl_Position = Model * vec4(iPos, 1.0);
}