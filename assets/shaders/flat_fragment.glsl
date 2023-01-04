#version 450

uniform vec4 uColor;

layout (location = 0) out vec4 fragColor;

in vec3 vNormal;

void main() {
   fragColor = vec4(vNormal, 1.0);
}
