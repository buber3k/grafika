#version 330 core

layout(location = 0) in vec3 iPos;
layout(location = 1) in vec2 iUV;

out vec3 fPos;
out vec2 fUV;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main() {
  gl_Position = Projection * View * Model * vec4(iPos, 1.0);
  fUV = iUV;
}