#version 450

uniform vec4 uColor;
uniform vec3 uCamPos;

layout (location = 0) out vec4 fragColor;

in vec3 vNormal;
in vec3 vPos;
in vec4 vLightPos;

void main() {
   vec3 lightDir = normalize(vLightPos.xyz - vPos);
   vec3 camDir = normalize(uCamPos - vPos);

   const float specular = 0.4;
   const float diffuse = 0.3;
   const float ambient = 0.1;
   const float shininess = 10.5;

   vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
   color += ambient * uColor;
   color += diffuse  * clamp(dot(lightDir, vNormal), 0.0, 1.0) * vec4(1.0); 
   color += specular * clamp(pow(dot(reflect(-lightDir, vNormal), camDir), shininess), 0.0, 1.0) * vec4(1.0); 
   color = color / color.w;

   fragColor = color;
}
