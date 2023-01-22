#version 450

uniform vec4 uColor;

layout (location = 0) out vec4 fragColor;

in vec3 vNormal;
in float vDepth;

void main() {
   float coef = 0.5; // / (vDepth * vDepth * 0.01 + 1.0);
   fragColor = mix(vec4(vNormal, 1.0), uColor, coef);
}
