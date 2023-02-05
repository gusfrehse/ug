#version 450

uniform vec4 uColor;

layout (location = 0) out vec4 fragColor;

in vec3 vNormal;
in float vDepth;

void main() {
   float coef = 1.0; // / (vDepth * vDepth * 0.01 + 1.0);
   fragColor = uColor;
}
