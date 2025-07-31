#pragma once

#include <QDialog>

namespace Ui {
class MeshingSetupDialog;
}

class MeshingSetupDialog : public QDialog
{
    Q_OBJECT

public slots:
    void accept(void);

public:
    explicit MeshingSetupDialog(QWidget * = 0);
    ~MeshingSetupDialog();
    void setData();

private slots:
    void changeRadius(int);
    void changeEdge(int);
    void changeChartDist(int);
    void changeLineLength(int);
    void changeCloseEdges(int);
    void changeSurfaceCurvature(int);
    void changeEdgeAngle(int);
    void changeSurfaceMeshCurv(int);

private:
    Ui::MeshingSetupDialog *ui;
    bool checkValues(void);
};
