#pragma once

#include <QDialog>

namespace Ui {
class CSGSetupDialog;
}

class CSGSetupDialog : public QDialog
{
    Q_OBJECT

public slots:
    void accept(void);

public:
    explicit CSGSetupDialog(double*, double*, double*, double*, QWidget* = 0);
    ~CSGSetupDialog();
    void setData();

private:
    double *minX{nullptr};
    double *maxX{nullptr};
    double *facets{nullptr};
    double *detail{nullptr};
    Ui::CSGSetupDialog *ui;
    bool checkValues(void);
};


