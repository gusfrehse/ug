#version 450
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

uniform vec4 uLightPos;

out vec3 vNormal;
out float vDepth;
out vec4 vLightPos;
out vec3 vPos;

void main() {
    gl_Position = uProjection * uView * uModel * aPos;
    vNormal = aNormal;
    vPos = aPos.xyz;
    vLightPos = uLightPos;
}
