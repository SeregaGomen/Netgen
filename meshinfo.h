#ifndef MESHINFO_H
#define MESHINFO_H

#include <QDialog>

namespace Ui {
class MeshInfo;
}

class MeshInfo : public QDialog
{
    Q_OBJECT

public:
    explicit MeshInfo(int, int, int, QWidget * = nullptr);
    ~MeshInfo();

private:
    Ui::MeshInfo *ui;
};

#endif // MESHINFO_H
