#version 330 core

layout(location = 0) in vec3 iPos;
layout(location = 1) in vec2 iUV;

out vec3 fPos;
out vec2 fUV;

void main() {
  gl_Position = vec4(iPos, 1.0);

  fPos = iPos;
  fUV = iUV;
}