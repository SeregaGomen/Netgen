#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>

struct ImageParams
{
    bool isAutoRotate{true};        // Автоповорот
    bool isMesh{true};              // Отображение сетки
    bool isSurface{true};           // ... поверхности
    bool isAxis{true};              // ... осей координат
    float angle[3]{0, 0, 0};        // Угол поворота вокруг осей Х, Y и Z
    float translate[3]{0, 0, 0};    // Сдвиг вдоль осей Х, Y и Z
    float alpha{1};                 // Прозрачность
    float scale{1};                 // Коэффициент масштабирования
    QColor bkgColor{"white"};       // Цвет фона изображения
};

class CSGView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    CSGView(void*, QWidget* = nullptr);
    virtual ~CSGView();
    void setRotating(bool isAutoRotate)
    {
        params.isAutoRotate = isAutoRotate;
    }
    bool getRotating(void)
    {
        return params.isAutoRotate;
    }
    ImageParams *getParams(void)
    {
        return &params;
    }
    virtual void refresh(void*);

protected slots:
    void slotRotate();

protected:
    int numVertex{0};
    float radius{0};
    float x0[3];
    void *geometry{nullptr};

    QMatrix4x4 projection;
    QMatrix4x4 view;
    QMatrix4x4 model;

    QOpenGLBuffer vbo[3];
    QOpenGLVertexArrayObject vao[3];
    QOpenGLShaderProgram shaderObject;
    QOpenGLShaderProgram shaderAxiss;

    ImageParams params;

    void initializeGL() override;
    void resizeGL(int, int) override;
    void paintGL() override;
    void wheelEvent(QWheelEvent*) override;
    virtual void showObject();
    virtual void calcRadius();
    virtual void initObject();

private:
    QTimer *timer;

    void initShaders();
    void initAxiss();
    void showAxiss();
    void showText(const QVector3D&, const QColor&, const QString&);
};

