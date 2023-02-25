#version 450

uniform vec4 uColor;

layout (location = 0) out vec4 fragColor;

in vec3 vNormal;
in float vDepth;
in vec4 vLightPos;

void main() {
   float coef = 0.5; // / (vDepth * vDepth * 0.01 + 1.0);
   fragColor = uColor * 1.0; //max(0.0, dot(vNormal, normalize(vLightPos.xyz)));
}
