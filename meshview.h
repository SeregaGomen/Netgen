#pragma once

#include "csgview.h"

class MeshView : public CSGView
{
private:
    int numMeshVertex{0};
    void showMesh();

protected:
    void initObject() override;
    void showObject() override;
    void calcRadius() override;

public:
    MeshView(void *geometry, QWidget *parent)
        : CSGView(geometry, parent)
    {}
    ~MeshView() = default;
};
