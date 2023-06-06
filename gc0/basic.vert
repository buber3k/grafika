#version 330 core

layout(location = 0) in vec3 iPos;

out vec3 fPos;

void main() {
  gl_Position = vec4(iPos, 1.0);

  fPos = iPos;
}