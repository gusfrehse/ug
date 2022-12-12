//
// Created by gus on 12/11/22.
//

#ifndef UG_RENDERABLE_H
#define UG_RENDERABLE_H

#include <GL/glew.h>

#include "Mesh.h"
#include "Material.h"

class Renderable {
private:
    Mesh *mMesh = nullptr;
    Material *mMaterial = nullptr;

public:
    Renderable(Mesh *mesh, Material *material) : mMesh{mesh}, mMaterial{material} {}
    Mesh* getMesh();
    Material* getMaterial();
};

#endif //UG_RENDERABLE_H
