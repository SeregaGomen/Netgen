#include <QColorDialog>
#include <cmath>

#include "csgview.h"
#include "imagesetup.h"
#include "meshview.h"
#include "ui_imagesetup.h"

ImageSetup::ImageSetup(ImageParams *params, CSGView *view, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ImageSetup)
    , params(params)
    , view(view)
{
    ui->setupUi(this);
    setup();

    connect(ui->hsTranslateX, SIGNAL(valueChanged(int)), this, SLOT(slotTranslateX(int)));
    connect(ui->hsTranslateY, SIGNAL(valueChanged(int)), this, SLOT(slotTranslateY(int)));
    connect(ui->hsAngleX, SIGNAL(valueChanged(int)), this, SLOT(slotRotateX(int)));
    connect(ui->hsAngleY, SIGNAL(valueChanged(int)), this, SLOT(slotRotateY(int)));
    connect(ui->hsAngleZ, SIGNAL(valueChanged(int)), this, SLOT(slotRotateZ(int)));
    connect(ui->hsScale, SIGNAL(valueChanged(int)), this, SLOT(slotScale(int)));
    connect(ui->pbColor, SIGNAL(clicked()), this, SLOT(slotChangeBkgColor()));

    connect(ui->rbMesh, SIGNAL(clicked()), this, SLOT(slotIsMesh()));
    connect(ui->rbSurface, SIGNAL(clicked()), this, SLOT(slotIsSurface()));
    connect(ui->rbSurfaceMesh, SIGNAL(clicked()), this, SLOT(slotIsSurfaceMesh()));
    connect(ui->chbAxis, SIGNAL(clicked()), this, SLOT(slotIsAxis()));
    connect(ui->hsAlpha, SIGNAL(valueChanged(int)), this, SLOT(slotChangeAlpha(int)));
    connect(ui->chbAutorotate, SIGNAL(clicked()), this, SLOT(slotIsAutoRotate()));
}

ImageSetup::~ImageSetup()
{
    delete ui;
}

void ImageSetup::slotIsAutoRotate(void)
{
    params->isAutoRotate = ui->chbAutorotate->isChecked();
    if (!params->isAutoRotate) {
        ui->gbRotate->setVisible(true);
        ui->hsAngleX->setValue(params->angle[0]);
        ui->hsAngleY->setValue(params->angle[1]);
        ui->hsAngleZ->setValue(params->angle[2]);
    } else
        ui->gbRotate->setVisible(false);
    // ui->hsAngleX->setEnabled(!params->isAutoRotate);
    // ui->hsAngleY->setEnabled(!params->isAutoRotate);
    // ui->hsAngleZ->setEnabled(!params->isAutoRotate);
    view->update();

    emit sendAutoRotateState(params->isAutoRotate);
}

void ImageSetup::slotIsMesh(void)
{
    params->isSurface = false;
    params->isMesh = true;
    view->update();

    emit sendShowMesh();
}

void ImageSetup::slotIsSurface(void)
{
    params->isSurface = true;
    params->isMesh = false;
    view->update();

    emit sendShowSurface();
}

void ImageSetup::slotIsSurfaceMesh(void)
{
    params->isSurface = params->isMesh = true;
    view->update();

    emit sendShowSurfaceAndMesh();
}

void ImageSetup::slotIsAxis(void)
{
    params->isAxis = ui->chbAxis->isChecked();
    view->update();
}

void ImageSetup::slotChangeAlpha(int value)
{
    ui->labelTransparency->setText(QString::number(float(value) / 10.0f, 'f', 1));
    params->alpha = float(value) / 10.0f;
    view->update();
}

void ImageSetup::slotChangeBkgColor(void)
{
    QColorDialog *cDlg = new QColorDialog(params->bkgColor, this);

    if (cDlg->exec() == QDialog::Accepted) {
        params->bkgColor = cDlg->currentColor();
        ui->pbColor->setStyleSheet(QString("background-color: rgb(%1, %2, %3)")
                                       .arg(params->bkgColor.red())
                                       .arg(params->bkgColor.green())
                                       .arg(params->bkgColor.blue()));
        view->update();
    }
    delete cDlg;
}

void ImageSetup::accept(void)
{
    QDialog::accept();
}

void ImageSetup::slotScale(int pos)
{
    ui->labelScale->setText(QString::number(float(pos) / 10.0f, 'f', 1));
    params->scale = float(pos) / 10.0f;
    view->update();
}

void ImageSetup::slotTranslateX(int pos)
{
    ui->labelTranslateX->setText(QString::number(float(pos) / 2.0f, 'f', 1));
    params->translate[0] = float(pos) / 2.0f;
    view->update();
}

void ImageSetup::slotTranslateY(int pos)
{
    ui->labelTranslateY->setText(QString::number(float(pos) / 2.0f, 'f', 1));
    params->translate[1] = float(pos) / 2.0f;
    view->update();
}

void ImageSetup::slotRotateX(int pos)
{
    ui->labelAngleX->setText(tr("%1").arg(pos));
    params->angle[0] = pos;
    view->update();
}

void ImageSetup::slotRotateY(int pos)
{
    ui->labelAngleY->setText(tr("%1").arg(pos));
    params->angle[1] = pos;
    view->update();
}

void ImageSetup::slotRotateZ(int pos)
{
    ui->labelAngleZ->setText(tr("%1").arg(pos));
    params->angle[2] = pos;
    view->update();
}

void ImageSetup::setup()
{
    ui->chbAxis->setChecked(params->isAxis);
    ui->chbAutorotate->setChecked(params->isAutoRotate);

    if (dynamic_cast<MeshView *>(view) == nullptr) {
        ui->rbMesh->setEnabled(false);
        ui->rbSurfaceMesh->setEnabled(false);
        ui->rbSurface->setEnabled(false);
    } else {
        ui->rbSurface->setChecked(params->isSurface);
        ui->rbMesh->setChecked(params->isMesh);
        ui->rbSurfaceMesh->setChecked(params->isMesh && params->isSurface);
    }

    ui->chbAutorotate->setChecked(params->isAutoRotate);
    if (params->isAutoRotate)
        ui->gbRotate->setVisible(false);
    else {
        ui->gbRotate->setVisible(true);
        // ui->labelAngleX->setText(QString::number(ui->hsAngleX->value()));
        // ui->labelAngleY->setText(QString::number(ui->hsAngleY->value()));
        // ui->labelAngleZ->setText(QString::number(ui->hsAngleZ->value()));
        ui->labelAngleX->setText(QString::number(params->angle[0]));
        ui->labelAngleY->setText(QString::number(params->angle[1]));
        ui->labelAngleZ->setText(QString::number(params->angle[2]));

        ui->hsAngleX->setValue(params->angle[0]);
        ui->hsAngleY->setValue(params->angle[1]);
        ui->hsAngleZ->setValue(params->angle[2]);
    }

    ui->hsAlpha->setValue(int(params->alpha * 10.0f));
    ui->labelTransparency->setText(QString::number(params->alpha, 'f', 1));
    ui->hsScale->setValue(int(params->scale * 10.0f));
    ui->labelScale->setText(QString::number(params->scale, 'f', 1));

    ui->hsTranslateX->setValue(int(params->translate[0] * 2.0f));
    ui->labelTranslateX->setText(QString::number(params->translate[0], 'f', 1));
    ui->hsTranslateY->setValue(int(params->translate[1] * 2.0f));
    ui->labelTranslateY->setText(QString::number(params->translate[1], 'f', 1));

    // Цвет фона
    ui->pbColor->setAutoFillBackground(true);
    ui->pbColor->setStyleSheet(QString("background-color: rgb(%1, %2, %3)")
                                   .arg(params->bkgColor.red())
                                   .arg(params->bkgColor.green())
                                   .arg(params->bkgColor.blue()));
}

//Стандартная формула (Rec. 601 / ITU-R BT.601)

//    gray = 0.299 * R + 0.587 * G + 0.114 * B
