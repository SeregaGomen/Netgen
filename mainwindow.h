#pragma once

#include <QMainWindow>
#include "defs.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QDockWidget;
class QProgressBar;
class QPlainTextEdit;
class StreamRedirector;
class QTimer;

namespace netgen {
class Mesh;
class NetgenGeometry;
}; // namespace netgen

const int maxRecentFiles = 5;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void slotOpen();
    void slotClose();
    void slotSave();
    void slotGenerateMesh();
    void slotStopMeshing();
    void slotCSGOptions();
    void slotScanCSG();
    void slotMeshingOptions();
    void slotViewGeometry();
    void slotViewMesh();
    void slotViewTerminal();
    void slotExportMesh();
    void slotRefineUniform();
    void slotImageSetup();
    void slotMeshInfo();
    // void slotRefineSecondOrder();
    // void slotRefineHighOrder();
    // void slotRefineBisection();

private slots:
    void slotOpenRecentFile();

protected:
    void closeEvent(QCloseEvent *);

private:
    ModelType modelType{ModelType::csg};
    double facets{20.0};
    double detail{0.001};
    double minX[3]{-1000, -1000, -1000};
    double maxX[3]{1000, 1000, 1000};

    bool isGenMeshStarted{false};
    bool isGenMeshCanceled{false};

    Ui::MainWindow *ui{nullptr};
    QStringList recentFiles;
    QAction *recentFileActs[maxRecentFiles];
    QProgressBar *pb{nullptr};
    QTimer *timer{nullptr};

    std::shared_ptr<netgen::NetgenGeometry> geometry{nullptr};
    std::shared_ptr<netgen::Mesh> mesh{nullptr};

    std::unique_ptr<StreamRedirector> ngCout{nullptr};
    std::unique_ptr<StreamRedirector> ngCerr{nullptr};

    QPlainTextEdit *terminal{nullptr};
    QDockWidget *dock{nullptr};

    void startProgress(int = 0, int = 0);
    void stopProgress();
    void checkMenuState();
    void readSettings();
    void writeSettings();
    void createRecentMenu();
    void setupRecentActions();
    void updateRecentFileActions(const QString &);
    void loadFile(const QString &);
    bool loadCSG(const QString &);
    bool loadSTL(const QString &);
    bool loadVOL(const QString &);
    void exportFile(const QString &);
    bool exportMesh(const QString &);
};
