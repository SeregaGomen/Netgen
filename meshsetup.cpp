#include "meshsetup.h"
#include <QMessageBox>
#include "stlgeom.hpp"
#include "ui_meshsetup.h"

using namespace netgen;

MeshingSetupDialog::MeshingSetupDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MeshingSetupDialog)
{
    ui->setupUi(this);

    connect(ui->hsRadius, SIGNAL(valueChanged(int)), this, SLOT(changeRadius(int)));
    connect(ui->hsEdge, SIGNAL(valueChanged(int)), this, SLOT(changeEdge(int)));
    connect(ui->hsChartDist, SIGNAL(valueChanged(int)), this, SLOT(changeChartDist(int)));
    connect(ui->hsLineLength, SIGNAL(valueChanged(int)), this, SLOT(changeLineLength(int)));

    connect(ui->hsCloseEdges, SIGNAL(valueChanged(int)), this, SLOT(changeCloseEdges(int)));
    connect(ui->hsSurfaceCurvature,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(changeSurfaceCurvature(int)));
    connect(ui->hsEdgeAngle, SIGNAL(valueChanged(int)), this, SLOT(changeEdgeAngle(int)));
    connect(ui->hsSurfaceMeshCurv,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(changeSurfaceMeshCurv(int)));
    setData();
    ui->buttonBox->setFocus();
}

MeshingSetupDialog::~MeshingSetupDialog()
{
    delete ui;
}

void MeshingSetupDialog::changeRadius(int pos)
{
    ui->laRadius->setText(tr("%1").arg(float(pos) * 0.2, 3, 'f', 1));
}

void MeshingSetupDialog::changeEdge(int pos)
{
    ui->laEdge->setText(tr("%1").arg(float(pos) * 0.2, 3, 'f', 1));
}

void MeshingSetupDialog::changeChartDist(int pos)
{
    ui->laChartDistance->setText(tr("%1").arg(float(pos) * 0.2, 3, 'f', 1));
}

void MeshingSetupDialog::changeLineLength(int pos)
{
    ui->laLineLength->setText(tr("%1").arg(float(pos) * 0.2, 3, 'f', 1));
}

void MeshingSetupDialog::changeCloseEdges(int pos)
{
    ui->laCloseEdges->setText(tr("%1").arg(float(pos) * 0.2, 3, 'f', 1));
}

void MeshingSetupDialog::changeSurfaceCurvature(int pos)
{
    ui->laSurfaceCurvature->setText(tr("%1").arg(float(pos) * 0.2, 3, 'f', 1));
}

void MeshingSetupDialog::changeEdgeAngle(int pos)
{
    ui->laEdgeAngle->setText(tr("%1").arg(float(pos) * 0.2, 3, 'f', 1));
}

void MeshingSetupDialog::changeSurfaceMeshCurv(int pos)
{
    ui->laSurfaceMeshCurv->setText(tr("%1").arg(float(pos) * 0.2, 3, 'f', 1));
}

void MeshingSetupDialog::setData()
{
    ui->leMin->setText(QString("%1").arg(mparam.minh));
    ui->leMax->setText(QString("%1").arg(mparam.maxh));
    ui->leGrading->setText(QString("%1").arg(mparam.grading));
    ui->hsRadius->setValue(int(mparam.curvaturesafety * 5));
    ui->hsEdge->setValue(int(mparam.segmentsperedge * 5));

    ui->hsChartDist->setValue(int(stlparam.resthchartdistfac * 5));
    ui->hsLineLength->setValue(int(stlparam.resthlinelengthfac * 5));
    ui->hsCloseEdges->setValue(int(stlparam.resthcloseedgefac * 5));
    ui->hsSurfaceCurvature->setValue(int(stlparam.resthsurfcurvfac * 5));
    ui->hsEdgeAngle->setValue(int(stlparam.yangle * 5));
    ui->hsSurfaceMeshCurv->setValue(int(stlparam.resthsurfmeshcurvfac * 5));
}

bool MeshingSetupDialog::checkValues(void)
{
    bool isOk;

    ui->leMin->text().toDouble(&isOk);
    if (!isOk) {
        ui->leMin->setFocus();
        return false;
    }
    ui->leMax->text().toDouble(&isOk);
    if (!isOk) {
        ui->leMax->setFocus();
        return false;
    }
    ui->leGrading->text().toDouble(&isOk);
    if (!isOk) {
        ui->leGrading->setFocus();
    }
    return isOk;
}

void MeshingSetupDialog::accept(void)
{
    if (checkValues()) {
        mparam.minh = ui->leMin->text().toDouble();
        mparam.maxh = ui->leMax->text().toDouble();
        mparam.grading = ui->leGrading->text().toDouble();
        mparam.curvaturesafety = double(ui->hsRadius->value()) * 0.2;
        mparam.segmentsperedge = double(ui->hsEdge->value()) * 0.2;
        stlparam.resthchartdistfac = double(ui->hsChartDist->value()) * 0.2;
        stlparam.resthlinelengthfac = double(ui->hsCloseEdges->value()) * 0.2;
        stlparam.resthsurfcurvfac = double(ui->hsSurfaceCurvature->value()) * 0.2;
        stlparam.yangle = double(ui->hsEdgeAngle->value()) * 0.2;
        stlparam.resthsurfmeshcurvfac = double(ui->hsSurfaceMeshCurv->value()) * 0.2;
        QDialog::accept();
    } else
        QMessageBox::information(this, tr("Error"), tr("Error floating value"));
}
