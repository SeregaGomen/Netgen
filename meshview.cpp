#include <QStatusBar>
#include <QMainWindow>
#include "stlgeom.hpp"
#include "meshview.h"

using namespace netgen;

void MeshView::initObject()
{
    int numTri = static_cast<Mesh*>(geometry)->GetNSE(), index[3];
    QVector<float> surfVertices, meshVertices, color{0.0f, 1.0f, 0.0f};
    auto getMeshNormal = [this](int i)
    {
        Element2d &face = static_cast<Mesh*>(geometry)->SurfaceElement(i);
        const Point3d &lp1 = static_cast<Mesh*>(geometry)->Point(face.PNum(1)),
                      &lp2 = static_cast<Mesh*>(geometry)->Point(face.PNum(2)),
                      &lp3 = static_cast<Mesh*>(geometry)->Point(face.PNum(3));
        Vec3d n = Cross (Vec3d (lp1, lp2), Vec3d (lp1, lp3));

        n /= (n.Length()+1e-12);
        return n;
    };

    surfVertices.reserve(numTri*27);
    meshVertices.reserve(numTri*36);
    for (int i = 1; i <= numTri; i++)
    {
        const Element2d &el = static_cast<Mesh*>(geometry)->SurfaceElement(i);

        auto normal = getMeshNormal(i);
        for (auto j = 1; j <= 3; j++)
        {
            const Point3d &p0 = static_cast<Mesh*>(geometry)->Point(el.PNum(j));

            surfVertices.append({float(p0.X()) - x0[0], float(p0.Y()) - x0[1], float(p0.Z()) - x0[2]});
            surfVertices.append({color[0], color[1], color[2]});
            surfVertices.append({float(normal.X()), float(normal.Y()), float(normal.Z())});

            meshVertices.append({float(p0.X()) - x0[0], float(p0.Y()) - x0[1], float(p0.Z()) - x0[2]});
            meshVertices.append({0, 0, 0});

            const Point3d &p1 = static_cast<Mesh*>(geometry)->Point(el.PNum(j < 3 ? j + 1 : 1));
            meshVertices.append({float(p1.X()) - x0[0], float(p1.Y()) - x0[1], float(p1.Z()) - x0[2]});
            meshVertices.append({0, 0, 0});

        }
        numVertex += 3;
        numMeshVertex += 6;
    }

    if (!vao[0].isCreated())
        vao[0].create();
    vao[0].bind();

    vbo[0].create();
    vbo[0].bind();
    vbo[0].allocate(surfVertices.data(), surfVertices.size()*sizeof(GLfloat));

    shaderObject.enableAttributeArray(0);
    shaderObject.setAttributeBuffer(0, GL_FLOAT, 0, 3, 9*sizeof(GLfloat));

    shaderObject.enableAttributeArray(1);
    shaderObject.setAttributeBuffer(1, GL_FLOAT, 3*sizeof(GLfloat), 3, 9*sizeof(GLfloat));

    shaderObject.enableAttributeArray(2);
    shaderObject.setAttributeBuffer(2, GL_FLOAT, 6*sizeof(GLfloat), 3, 9*sizeof(GLfloat));

    vbo[0].release();
    vao[0].release();

    if (!vao[1].isCreated())
        vao[1].create();
    vao[1].bind();

    if (!vbo[1].isCreated())
        vbo[1].create();
    vbo[1].bind();
    vbo[1].allocate(meshVertices.data(), meshVertices.size()*sizeof(GLfloat));

    shaderObject.enableAttributeArray(0);
    shaderObject.setAttributeBuffer(0, GL_FLOAT, 0, 3, 6*sizeof(GLfloat));

    shaderObject.enableAttributeArray(1);
    shaderObject.setAttributeBuffer(1, GL_FLOAT, 3*sizeof(GLfloat), 3, 6*sizeof(GLfloat));


    vbo[1].release();
    vao[1].release();
}

void MeshView::showMesh()
{
    auto numPoints = static_cast<Mesh*>(geometry)->GetNP(),
         numElements = static_cast<Mesh*>(geometry)->GetNE(),
         numSurfElemets = static_cast<Mesh*>(geometry)->GetNSE();

    shaderObject.bind();
    shaderObject.setUniformValue("model", model);
    shaderObject.setUniformValue("view", view);
    shaderObject.setUniformValue("projection", projection);
    shaderObject.setUniformValue("lightPos", QVector3D(0.5f, 0.7f, 1.0f));
    shaderObject.setUniformValue("scale", QVector4D(params.scale, params.scale, params.scale, 1.0f));
    shaderObject.setUniformValue("alpha", params.alpha);
    shaderObject.setUniformValue("isLight", true);
    shaderObject.setUniformValue("translation", QVector4D(params.translate[0]*radius/10.0f, params.translate[1]*radius/10.0f, 0.0f, 0.0f));
    vao[1].bind();
    glDrawArrays(GL_LINES, 0, numMeshVertex);
    vao[1].release();
    shaderObject.release();


    static_cast<QMainWindow*>(parent())->statusBar()->showMessage(tr("Points: %1\t\t Elements: %2\t\t Surf Elements: %3").arg(numPoints).arg(numElements).arg(numSurfElemets));
}


void MeshView::showObject()
{
    if (params.isSurface)
        CSGView::showObject();
    if (params.isMesh)
        showMesh();
}

void MeshView::calcRadius()
{
    double minX, maxX, minY, maxY, minZ, maxZ;
    const Point3d &p = static_cast<Mesh*>(geometry)->Point(1);

    minX = maxX = p.X();
    minY = maxY = p.Y();
    minZ = maxZ = p.Z();
    for (auto i = 2; i <= static_cast<Mesh*>(geometry)->GetNP(); i++)
    {
        const Point3d &p = static_cast<Mesh*>(geometry)->Point(i);
        minX = p.X() < minX ? p.X() : minX;
        maxX = p.X() > maxX ? p.X() : maxX;
        minY = p.Y() < minY ? p.Y() : minY;
        maxY = p.Y() > maxY ? p.Y() : maxY;
        minZ = p.Z() < minZ ? p.Z() : minZ;
        maxZ = p.Z() > maxZ ? p.Z() : maxZ;
    }
    x0[0] = (minX + maxX)*0.5f;
    x0[1] = (minY + maxY)*0.5f;
    x0[2] = (minZ + maxZ)*0.5f;
    radius = sqrt(pow(maxX - minX, 2) + pow(maxY - minY, 2) + pow(maxZ - minZ, 2));
}
