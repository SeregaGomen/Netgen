#pragma once

#include <QDialog>

class CSGView;
struct ImageParams;

namespace Ui {
    class ImageSetup;
}

using namespace std;

enum class ImageType { mesh, func, param };

class ImageSetup : public QDialog
{
    Q_OBJECT

public slots:
    void accept(void);

public:
    explicit ImageSetup(ImageParams*, CSGView*, QWidget * = nullptr);
    ~ImageSetup();

private slots:
    void slotScale(int);
    void slotChangeBkgColor(void);
    void slotRotateX(int);
    void slotRotateY(int);
    void slotRotateZ(int);
    void slotTranslateX(int);
    void slotTranslateY(int);
    void slotIsMesh(void);
    void slotIsSurface(void);
    void slotIsSurfaceMesh(void);
    void slotIsAxis(void);
    void slotChangeAlpha(int);
    void slotIsAutoRotate(void);

signals:
    void sendAutoRotateState(bool);
    void sendShowSurface();
    void sendShowMesh();
    void sendShowSurfaceAndMesh();


private:
    Ui::ImageSetup *ui;
    ImageParams *params = nullptr;
    CSGView* view = nullptr;
    void setup();

};
