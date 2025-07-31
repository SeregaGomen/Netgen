#include "meshinfo.h"
#include "ui_meshinfo.h"

MeshInfo::MeshInfo(int points, int elements, int surfElements, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MeshInfo)
{
    ui->setupUi(this);
    ui->lbPoints->setText(QString("%1").arg(points));
    ui->lbElements->setText(QString("%1").arg(elements));
    ui->lbSurfElements->setText(QString("%1").arg(surfElements));
}

MeshInfo::~MeshInfo()
{
    delete ui;
}
