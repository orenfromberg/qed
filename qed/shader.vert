#version 130

in vec3 vertex;
in vec2 texCoord;

uniform mat4x4 mvp;

out vec2 tCoord;

void main ()
{
  gl_Position = mvp * vec4(vertex, 1.0);
  tCoord = texCoord.st;
}
