 // Indices into vertex buffer
std::vector<unsigned int>  index; 
void Object::renderShadowCaps() {
// (The "Culling" optimization changes this method 
// to try to cull the light and dark caps separately.)
  index.resize(0);
  for (int f = face.size() - 1; f >= 0; --f) {
    if (backface[f]) {
      // Dark cap (same vertices but at infinity)
    for (int v = 0; v < 3; ++v) {
        index.pushBack(face[f].vertex[v] + n);
      }
    } else {
        // Light cap
    for (int v = 0; v < 3; ++v) {
          index.pushBack(face[f].vertex[v]);
        }
      }
  } 
  glDrawElements(GL_TRIANGLES, index.size(),
                 GL_UNSIGNED_INT, index.begin()); 
} 

void Object::renderShadowSides() {
  index.resize(0); 
  for (int e = edges.size() - 1; e >= 0; --e) {
    if (backface[edge[e].face[0]] != backface[edge[e].face[1]) {
      // This is a silhouette edge
    int v0, v1;
      if (backface[edge[e].face[0])) {
        // Wind the same way as face 0
        v0 = edge[e].vertex[0];
        v1 = edge[e].vertex[1];
      } else {
          // Wind the same way as face 1
          v1 = edge[e].vertex[0];
          v0 = edge[e].vertex[1];
        } 
      // (The "directional" optimization changes this code.)
      index.pushBack(v0);
      index.pushBack(v1);
      index.pushBack(v1 + n);
      index.pushBack(v0 + n);
    }
  } 

  // (The "directional" optimization changes this to use 
  // GL_TRIANGLES instead of GL_QUADS.)
  glDrawElements(GL_QUADS, index.size(),
                 GL_UNSIGNED_INT, index.begin());
} 

VOut main(const float4x4 uniform in MVP,
          const float4 uniform in L,
          const VIn in vin) {
  VOut vout;
    // (The "directional" optimization eliminates the vertex shader 
    // by using different rendering loops for point and directional 
    // lights.)
  vout.pos = MVP * (vin.pos.w == 0 ?
                    float4(vin.pos.xyz * L.w - L.xyz, 0) :
                    vin.posvin.pos); 
  return vout;
} 
