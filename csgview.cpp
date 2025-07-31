#include "csgview.h"
#include <QOpenGLShader>
#include <QPainter>
#include <QTimer>
#include <QWheelEvent>
#include "csg.hpp"

using namespace netgen;

CSGView::CSGView(void *geometry, QWidget *parent)
    : QOpenGLWidget(parent)
    , geometry(geometry)
{
    for (auto i = 0; i < 3; i++)
        vbo[i] = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&CSGView::slotRotate));
    if (geometry)
        timer->start(16); // ~60 FPS
}

CSGView::~CSGView()
{
    makeCurrent();
    for (auto i = 0; i < 3; i++) {
        vbo[i].destroy();
        vao[i].destroy();
    }
    doneCurrent();
    delete timer;
}

void CSGView::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    initShaders();
    if (geometry) {
        calcRadius();
        initObject();
        initAxiss();
    }
    view.lookAt(QVector3D(radius, radius, radius),
                QVector3D(0.0f, 0.0f, 0.0f),
                QVector3D(0.0f, 0.0f, radius));
    model.setToIdentity();
}

void CSGView::slotRotate()
{
    if (params.isAutoRotate) {
        params.angle[0] = params.angle[0] < 360.0 ? params.angle[0] + 1.0 : params.angle[0] - 360.0;
        params.angle[1] = params.angle[1] < 360.0 ? params.angle[1] + 0.7 : params.angle[1] - 360.0;
        params.angle[2] = params.angle[2] < 360.0 ? params.angle[2] + 0.3 : params.angle[2] - 360.0;
        update();
    }
}

void CSGView::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    projection.setToIdentity();
    projection.perspective(45.0f, float(w) / float(h), 0.01f * radius, 10.0f * radius);
}

void CSGView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(params.bkgColor.redF(),
                 params.bkgColor.greenF(),
                 params.bkgColor.blueF(),
                 params.bkgColor.alphaF());
    //glClearColor(0, 0, 0, 1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f); // Слегка отодвигаем назадglEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    model.setToIdentity();
    model.rotate(params.angle[0], 1, 0, 0);
    model.rotate(params.angle[1], 0, 1, 0);
    model.rotate(params.angle[2], 0, 0, 1);

    if (geometry) {
        showObject();
        if (params.isAxis)
            showAxiss();
    }
}

void CSGView::showObject()
{
    shaderObject.bind();
    shaderObject.setUniformValue("model", model);
    shaderObject.setUniformValue("view", view);
    shaderObject.setUniformValue("projection", projection);
    shaderObject.setUniformValue("lightPos", QVector3D(0.5f, 0.7f, 1.0f));
    shaderObject.setUniformValue("scale", QVector4D(params.scale, params.scale, params.scale, 1.0f));
    shaderObject.setUniformValue("alpha", params.alpha);
    shaderObject.setUniformValue("isLight", true);
    shaderObject.setUniformValue("translation",
                                 QVector4D(params.translate[0] * radius / 10.0f,
                                           params.translate[1] * radius / 10.0f,
                                           0.0f,
                                           0.0f));
    vao[0].bind();
    glDrawArrays(GL_TRIANGLES, 0, numVertex);
    vao[0].release();
    shaderObject.release();
}

void CSGView::showAxiss()
{
    auto worldToScreen = [this](const QVector3D &worldPos,
                                const QMatrix4x4 &model,
                                const QMatrix4x4 &view,
                                const QMatrix4x4 &projection,
                                const QVector4D &translation) {
        QMatrix4x4 mvp = projection * view * model;
        QVector4D clipSpacePos = mvp * QVector4D(worldPos, 1.0) - translation;

        if (clipSpacePos.w() == 0.0f)
            return QVector3D(); // ошибка деления

        // NDC [-1, 1]
        QVector3D ndc = clipSpacePos.toVector3DAffine(); // делит на w

        // Viewport transform -> [0, width], [0, height]
        float x = (ndc.x() * 0.5f + 0.5f) * width();
        float y = (1.0f - (ndc.y() * 0.5f + 0.5f)) * height(); // инверсия Y
        float z = ndc.z();

        return QVector3D(x, y, z);
    };

    shaderAxiss.bind();
    shaderAxiss.setUniformValue("model", model);
    shaderAxiss.setUniformValue("view", view);
    shaderAxiss.setUniformValue("projection", projection);
    shaderAxiss.setUniformValue("translation", QVector4D(1.6f * radius, 1.4f * radius, 0.0f, 0.0f));
    shaderAxiss.setUniformValue("scale", QVector4D(1, 1, 1, 0.0f));
    shaderAxiss.setUniformValue("alpha", 1.0f);
    vao[2].bind();
    glDrawArrays(GL_LINES, 0, 6);
    vao[2].release();
    shaderAxiss.release();

    showText(worldToScreen(QVector3D(0.06 * radius, 0.0, 0.0),
                           model,
                           view,
                           projection,
                           QVector4D(1.6f * radius, 1.4f * radius, 0.0f, 0.0f)),
             Qt::red,
             "X");
    showText(worldToScreen(QVector3D(0.0, 0.06 * radius, 0.0),
                           model,
                           view,
                           projection,
                           QVector4D(1.6f * radius, 1.4f * radius, 0.0f, 0.0f)),
             Qt::green,
             "Y");
    showText(worldToScreen(QVector3D(0.0, 0.0, 0.06 * radius),
                           model,
                           view,
                           projection,
                           QVector4D(1.6f * radius, 1.4f * radius, 0.0f, 0.0f)),
             Qt::blue,
             "Z");
}

void CSGView::showText(const QVector3D &pos, const QColor &color, const QString &text)
{
    QPainter painter(this);

    painter.setFont(QFont("Aral", 8));
    painter.setPen(color);
    painter.drawText(pos.x(), pos.y(), text);
    painter.setPen(Qt::green);
    painter.end();
}

void CSGView::initShaders()
{
    // shaderObject.addShaderFromSourceCode(QOpenGLShader::Vertex,
    //     R"(
    //         #version 330 core
    //         layout(location = 0) in vec3 aPos;
    //         layout(location = 1) in vec3 aColor;
    //         layout(location = 2) in vec3 aNormal;

    //         uniform mat4 model;
    //         uniform mat4 view;
    //         uniform mat4 projection;

    //         out vec3 FragPos;
    //         out vec3 Normal;
    //         out vec3 Color;

    //         void main()
    //         {
    //             FragPos = vec3(model*vec4(aPos, 1.0));
    //             Normal = mat3(transpose(inverse(model)))*aNormal;
    //             Color = aColor;
    //             gl_Position = projection*view*vec4(FragPos, 1.0);
    //         }
    //     )");

    // shaderObject.addShaderFromSourceCode(QOpenGLShader::Fragment,
    //     R"(
    //         #version 330 core
    //         in vec3 Normal;
    //         in vec3 Color;

    //         uniform vec3 lightPos;

    //         out vec4 FragColor;

    //         void main()
    //         {
    //             FragColor = vec4(Color, 1);
    //             FragColor.rgb *= abs(dot(Normal, lightPos));
    //         }
    //     )");
    // shaderObject.link();

    // shaderAxiss.addShaderFromSourceCode(QOpenGLShader::Vertex,
    //     R"(
    //         #version 330 core
    //         layout(location = 0) in vec3 aPos;
    //         layout(location = 1) in vec3 aColor;

    //         uniform mat4 model;
    //         uniform mat4 view;
    //         uniform mat4 projection;
    //         uniform vec4 translation;

    //         out vec3 FragPos;
    //         out vec3 Color;

    //         void main()
    //         {
    //             FragPos = vec3(model*vec4(aPos, 1.0));
    //             Color = aColor;
    //             gl_Position = projection*view*vec4(FragPos, 1.0) - translation;
    //         }
    //     )");
    // shaderAxiss.addShaderFromSourceCode(QOpenGLShader::Fragment,
    //     R"(
    //         #version 330 core
    //         in vec3 Color;

    //         out vec4 FragColor;

    //         void main()
    //         {
    //             FragColor = vec4(Color, 1);
    //         }
    //     )");
    // shaderAxiss.link();

    shaderObject.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                         R"(
            #version 330 core
            layout(location = 0) in vec3 aPos;
            layout(location = 1) in vec3 aColor;
            layout(location = 2) in vec3 aNormal;

            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;
            uniform vec4 scale;
            uniform vec4 translation;

            out vec3 FragPos;
            out vec3 Normal;
            out vec3 Color;

            void main()
            {
                FragPos = vec3(model*vec4(aPos, 1.0)*scale);
                Normal = mat3(transpose(inverse(model)))*aNormal;
                Color = aColor;
                gl_Position = projection*view*vec4(FragPos, 1.0) + translation;
            }
        )");

    shaderObject.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                         R"(
            #version 330 core
            in vec3 Normal;
            in vec3 Color;

            uniform vec3 lightPos;
            uniform float alpha;
            uniform bool isLight;

            out vec4 FragColor;

            void main()
            {
                FragColor = vec4(Color, alpha);
                FragColor.rgb *= isLight ? abs(dot(Normal, lightPos)) : 1.0;
            }
        )");
    shaderObject.link();

    shaderAxiss.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                        R"(
            #version 330 core
            layout(location = 0) in vec3 aPos;
            layout(location = 1) in vec3 aColor;

            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;
            uniform vec4 translation;

            out vec3 FragPos;
            out vec3 Color;

            void main()
            {
                FragPos = vec3(model*vec4(aPos, 1.0));
                Color = aColor;
                gl_Position = projection*view*vec4(FragPos, 1.0) - translation;
            }
        )");
    shaderAxiss.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                        R"(
            #version 330 core
            in vec3 Color;

            out vec4 FragColor;

            void main()
            {
                FragColor = vec4(Color, 1);
            }
        )");
    shaderAxiss.link();
}

void CSGView::initAxiss()
{
    double rgb[][3]{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
    QVector<float> vertices;

    vertices.reserve(36);
    vertices.append({0, 0, 0, 1, 0, 0, 0.05f * radius, 0, 0, 1, 0, 0});
    vertices.append({0, 0, 0, 0, 1, 0, 0, 0.05f * radius, 0, 0, 1, 0});
    vertices.append({0, 0, 0, 0, 0, 1, 0, 0, 0.05f * radius, 0, 0, 1});

    vao[2].create();
    vao[2].bind();

    vbo[2].create();
    vbo[2].bind();
    vbo[2].allocate(vertices.data(), 36 * sizeof(GLfloat));

    shaderAxiss.enableAttributeArray(0);
    shaderAxiss.setAttributeBuffer(0, GL_FLOAT, 0, 3, 6 * sizeof(GLfloat));
    shaderAxiss.enableAttributeArray(1);
    shaderAxiss.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 3, 6 * sizeof(GLfloat));

    vbo[2].release();
    vao[2].release();
}

void CSGView::initObject()
{
    int size{0};
    double rgb[]{0.7, 0.2, 0.3};
    QVector<float> vertices, color{0.0f, 0.0f, 1.0f};

    for (auto i = 0; i < static_cast<CSGeometry *>(geometry)->GetNTopLevelObjects(); i++)
        for (int j = 0; j < static_cast<CSGeometry *>(geometry)->GetTriApprox(i)->GetNT(); j++)
            size++;
    vertices.reserve(size * 27);

    for (auto i = 0; i < static_cast<CSGeometry *>(geometry)->GetNTopLevelObjects(); i++) {
        const TriangleApproximation &ta = *static_cast<CSGeometry *>(geometry)->GetTriApprox(i);

        for (int j = 0; j < ta.GetNT(); j++) {
            for (int k = 0; k < 3; k++) {
                int pi = ta.GetTriangle(j)[k];

                // coordinates
                vertices.append({float(ta.GetPoint(pi)(0) - x0[0]),
                                 float(ta.GetPoint(pi)(1) - x0[1]),
                                 float(ta.GetPoint(pi)(2) - x0[2])});
                // colors
                vertices.append(color);
                // normals
                vertices.append({float(ta.GetNormal(pi)(0)),
                                 float(ta.GetNormal(pi)(1)),
                                 float(ta.GetNormal(pi)(2))});
            }
            numVertex += 3;
        }
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

void CSGView::calcRadius()
{
    float minX[3]{0, 0, 0}, maxX[3]{0, 0, 0};

    for (auto i = 0; i < static_cast<CSGeometry *>(geometry)->GetNTopLevelObjects(); i++) {
        const TriangleApproximation &ta = *static_cast<CSGeometry *>(geometry)->GetTriApprox(i);

        if (i == 0)
            for (auto j = 0; j < 3; j++)
                minX[j] = maxX[j] = ta.GetPoint(ta.GetTriangle(0)[0])(j);
        for (auto j = 1; j < ta.GetNT(); j++)
            for (auto k = 0; k < 3; k++) {
                auto pi = ta.GetTriangle(j)[k];

                for (auto l = 0; l < 3; l++) {
                    minX[l] = (ta.GetPoint(pi)(l) < minX[l]) ? ta.GetPoint(pi)(l) : minX[l];
                    maxX[l] = (ta.GetPoint(pi)(l) > maxX[l]) ? ta.GetPoint(pi)(l) : maxX[l];
                }
            }
    }
    for (auto i = 0; i < 3; i++)
        x0[i] = (minX[i] + maxX[i]) * 0.5f;

    radius = sqrt(pow(maxX[0] - minX[0], 2) + pow(maxX[1] - minX[1], 2) + pow(maxX[2] - minX[2], 2));
}

void CSGView::refresh(void *geometry)
{
    this->geometry = geometry;
    initObject();
    update();
}

void CSGView::wheelEvent(QWheelEvent *pe)
{
    if (pe->angleDelta().x() > 0 or pe->angleDelta().y() > 0)
        params.scale = params.scale * 1.05f > 5.0f ? 5.0f : params.scale * 1.05f;
    else
        params.scale = params.scale / 1.05f < 0.5f ? 0.5f : params.scale / 1.05f;
    update();
}
