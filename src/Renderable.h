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
    const Mesh *mMesh = nullptr;
    const Material *mMaterial = nullptr;

public:
    Renderable(Mesh *mesh, Material *material) : mMesh{mesh}, mMaterial{material} {}
    const Mesh* getMesh() const;
    const Material* getMaterial() const;
};

#endif //UG_RENDERABLE_H
