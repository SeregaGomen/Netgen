#include "stlview.h"
#include "stlgeom.hpp"

using namespace netgen;

void STLView::initObject()
{
    int numTri = static_cast<STLGeometry *>(geometry)->GetNT();
    QVector<float> vertices, color{0.0f, 0.0f, 1.0f};

    vertices.reserve(numTri * 27);
    for (auto i = 1; i <= numTri; i++) {
        Vec3d normal = ((STLGeometry *) geometry)->GetTriangle(i).Normal();

        for (auto j = 1; j <= 3; j++) {
            const Point3d &tp = ((STLGeometry *) geometry)
                                    ->GetPoint(((STLGeometry *) geometry)->GetTriangle(i).PNum(j));

            vertices.append({float(tp.X()) - x0[0], float(tp.Y()) - x0[1], float(tp.Z()) - x0[2]});
            vertices.append({color[0], color[1], color[2]});
            vertices.append({float(normal.X()), float(normal.Y()), float(normal.Z())});
        }
        numVertex += 3;
    }

    if (!vao[0].isCreated())
        vao[0].create();
    vao[0].bind();

    vbo[0].create();
    vbo[0].bind();
    vbo[0].allocate(vertices.data(), vertices.size() * sizeof(GLfloat));

    shaderObject.enableAttributeArray(0);
    shaderObject.setAttributeBuffer(0, GL_FLOAT, 0, 3, 9 * sizeof(GLfloat));

    shaderObject.enableAttributeArray(1);
    shaderObject.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 3, 9 * sizeof(GLfloat));

    shaderObject.enableAttributeArray(2);
    shaderObject.setAttributeBuffer(2, GL_FLOAT, 6 * sizeof(GLfloat), 3, 9 * sizeof(GLfloat));

    vbo[0].release();
    vao[0].release();
}

void STLView::calcRadius()
{
    float minX[3], maxX[3];

    minX[0] = maxX[0] = ((STLGeometry *) geometry)->GetPoint(1)(0);
    minX[1] = maxX[1] = ((STLGeometry *) geometry)->GetPoint(1)(1);
    minX[2] = maxX[2] = ((STLGeometry *) geometry)->GetPoint(1)(2);
    for (int i = 2; i <= static_cast<STLGeometry *>(geometry)->GetNP(); i++)
        for (auto j = 0; j < 3; j++) {
            minX[j] = static_cast<STLGeometry *>(geometry)->GetPoint(i)(j) < minX[j]
                          ? static_cast<STLGeometry *>(geometry)->GetPoint(i)(j)
                          : minX[j];
            maxX[j] = static_cast<STLGeometry *>(geometry)->GetPoint(i)(j) > maxX[j]
                          ? static_cast<STLGeometry *>(geometry)->GetPoint(i)(j)
                          : maxX[j];
        }
    for (auto i = 0; i < 3; i++)
        x0[i] = (minX[i] + maxX[i]) * 0.5f;
    radius = sqrt(pow(maxX[0] - minX[0], 2) + pow(maxX[1] - minX[1], 2) + pow(maxX[2] - minX[2], 2));
}
