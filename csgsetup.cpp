#include <QMessageBox>
#include "csgsetup.h"
#include "ui_csgsetup.h"

CSGSetupDialog::CSGSetupDialog(double *minX, double *maxX, double *facets, double *detail, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CSGSetupDialog), minX(minX), maxX(maxX), facets(facets), detail(detail)
{
    ui->setupUi(this);
    setData();
    ui->buttonBox->setFocus();
}

CSGSetupDialog::~CSGSetupDialog()
{
    delete ui;
}

void CSGSetupDialog::setData()
{
    ui->leFacets->setText(QString("%1").arg(*facets));
    ui->leDetail->setText(QString("%1").arg(*detail));
    ui->leMinX->setText(QString("%1").arg(minX[0]));
    ui->leMaxX->setText(QString("%1").arg(maxX[0]));
    ui->leMinY->setText(QString("%1").arg(minX[1]));
    ui->leMaxY->setText(QString("%1").arg(maxX[1]));
    ui->leMinZ->setText(QString("%1").arg(minX[2]));
    ui->leMaxZ->setText(QString("%1").arg(maxX[2]));
}

bool CSGSetupDialog::checkValues(void)
{
    bool isOk;

    ui->leFacets->text().toDouble(&isOk);
    if (!isOk) {
        ui->leFacets->setFocus();
        return false;
    }
    ui->leDetail->text().toDouble(&isOk);
    if (!isOk) {
        ui->leDetail->setFocus();
        return false;
    }
    ui->leMinX->text().toDouble(&isOk);
    if (!isOk) {
        ui->leMinX->setFocus();
        return false;
    }
    ui->leMaxX->text().toDouble(&isOk);
    if (!isOk) {
        ui->leMaxX->setFocus();
        return false;
    }
    ui->leMinY->text().toDouble(&isOk);
    if (!isOk) {
        ui->leMinY->setFocus();
        return false;
    }
    ui->leMaxY->text().toDouble(&isOk);
    if (!isOk) {
        ui->leMaxY->setFocus();
        return false;
    }
    ui->leMinZ->text().toDouble(&isOk);
    if (!isOk) {
        ui->leMinZ->setFocus();
        return false;
    }
    ui->leMaxZ->text().toDouble(&isOk);
    if (!isOk) {
        ui->leMaxZ->setFocus();
        return false;
    }
    return isOk;
}

void CSGSetupDialog::accept(void)
{
    if (checkValues())
    {
        *facets = ui->leFacets->text().toDouble();
        *detail = ui->leDetail->text().toDouble();
        minX[0] = ui->leMinX->text().toDouble();
        maxX[0] = ui->leMaxX->text().toDouble();
        minX[1] = ui->leMinY->text().toDouble();
        maxX[1] = ui->leMaxY->text().toDouble();
        minX[2] = ui->leMinZ->text().toDouble();
        maxX[2] = ui->leMaxZ->text().toDouble();
        QDialog::accept();
    }
    else
        QMessageBox::information(this, tr("Error"), tr("Error floating value"));
}
