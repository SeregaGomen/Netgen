#include <QCloseEvent>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QSettings>
#include <QTimer>

#include "./ui_mainwindow.h"
#include "csgsetup.h"
#include "csgview.h"
#include "imagesetup.h"
#include "mainwindow.h"
#include "meshsetup.h"
#include "meshview.h"
#include "ng.h"
#include "redirector.h"
#include "stlview.h"
#include "threads.h"
#include "meshinfo.h"

using namespace netgen;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Для предотвращение кратковременного исчезновения окна при первом отображении изображения
    setCentralWidget(new CSGView(nullptr, this));

    setupRecentActions();
    readSettings();
    createRecentMenu();

    pb = new QProgressBar(statusBar());
    pb->setTextVisible(false);
    pb->hide();
    statusBar()->addPermanentWidget(pb);

    timer = new QTimer(this);
    pb->setTextVisible(true);
    connect(timer, &QTimer::timeout, this, [=]() {
        pb->setValue(multithread.percent);
        pb->setFormat(multithread.task);
    });

    terminal = new QPlainTextEdit(this);
    //terminal->setTextColor(QColor( "black" ));
    terminal->setStyleSheet("QTextEdit { background-color: rgb(255, 255, 255) }");
    terminal->setReadOnly(true);
    terminal->setWordWrapMode(QTextOption::NoWrap);
    terminal->setFont(QFont("Courier"));

    //////////////////////
    dock = new QDockWidget(tr("Terminal"), this);
    dock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    dock->setWidget(terminal);
    dock->setVisible(ui->actionViewTerminal->isChecked());
    dock->setWindowTitle(tr("Terminal"));
    addDockWidget(Qt::BottomDockWidgetArea, dock);

    //////////////////////
    // Перехват cout и cerr
    ngCout = make_unique<StreamRedirector>(terminal, std::cout);
    ngCerr = make_unique<StreamRedirector>(terminal, std::cerr, Qt::red);

    // auto checkMenu = [=]() { checkMenuState(); };
    // connect(ui->menu_File, &QMenu::aboutToShow, this, checkMenu);
    // connect(ui->menu_View, &QMenu::aboutToShow, this, checkMenu);
    // connect(ui->menu_Geometry, &QMenu::aboutToShow, this, checkMenu);
    // connect(ui->menu_Mesh, &QMenu::aboutToShow, this, checkMenu);
    checkMenuState();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotOpen(void)
{
    bool rotating = static_cast<CSGView *>(centralWidget())
    ? static_cast<CSGView *>(centralWidget())->getRotating()
    : false;

    if (rotating)
        static_cast<CSGView *>(centralWidget())->setRotating(false);
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open"),
        "",
        tr("All Geometry Types (*.geo *.stl ...);; Geometry file (*.geo);; STL Geometry (*.stl);; "
           "Binary STL Geometry (*.stlb);; Mesh file (*.vol *vol.gz)"));
    if (rotating)
        static_cast<CSGView *>(centralWidget())->setRotating(true);

    if (fileName.length())
        loadFile(fileName);

    checkMenuState();
}

void MainWindow::loadFile(const QString &fileName)
{
    bool ok{false};

    startProgress(0, 100);
    if (QFileInfo(fileName).suffix().toUpper() == "GEO")
        ok = loadCSG(fileName);
    else if (QFileInfo(fileName).suffix().toUpper() == "STL")
        ok = loadSTL(fileName);
    else if (QFileInfo(fileName).suffix().toUpper() == "VOL"
             || QFileInfo(fileName).completeSuffix().toUpper() == "VOL.GZ")
        ok = loadVOL(fileName);
    stopProgress();

    if (ok) {
        statusBar()->showMessage(tr("File successfully downloaded"), 5000);
        updateRecentFileActions(fileName);
    } else {
        statusBar()->showMessage(tr("Error opening file"), 5000);
        QMessageBox(QMessageBox::Critical, tr("Error"), tr("Error loading file")).exec();
    }
    cout << endl;
}

bool MainWindow::loadCSG(const QString &fileName)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (!::loadCSG(fileName.toStdString(), geometry, minX, maxX, detail, facets)) {
        QApplication::restoreOverrideCursor();
        statusBar()->showMessage(tr("Error reading in current CSG data"), 5000);
        return false;
    }
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Successfully loaded CSG data"), 5000);
    modelType = ModelType::csg;
    mesh = nullptr;
    setCentralWidget(new CSGView(static_cast<CSGeometry *>(geometry.get()), this));
    setWindowTitle(QFileInfo(fileName).fileName() + " - Netgen");
    return true;
}

bool MainWindow::loadSTL(const QString &fileName)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (!::loadSTL(fileName.toStdString(), geometry)) {
        QApplication::restoreOverrideCursor();
        statusBar()->showMessage(tr("Error reading in current STL data"), 5000);
        return false;
    }
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Successfully loaded STL data"), 5000);
    modelType = ModelType::stl;
    mesh = nullptr;
    setCentralWidget(new STLView(static_cast<STLGeometry *>(geometry.get()), this));
    setWindowTitle(QFileInfo(fileName).fileName() + " - Netgen");
    return true;
}

bool MainWindow::loadVOL(const QString &fileName)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (!::loadMesh(fileName.toStdString(), mesh)) {
        QApplication::restoreOverrideCursor();
        statusBar()->showMessage(tr("Error reading in current VOL-file"), 5000);
        return false;
    }
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Successfully loaded mesh data"), 5000);
    modelType = ModelType::mesh;
    geometry = nullptr;
    setCentralWidget(new MeshView(mesh.get(), this));
    setWindowTitle(QFileInfo(fileName).fileName() + " - Netgen");
    return true;
}

void MainWindow::readSettings()
{
    QSettings settings("NETGEN", "Netgen");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    QString path = settings.value("path").toString();
    int states = settings.value("state").toInt();

    recentFiles = settings.value("recentFileList").toStringList();
    if (Qt::WindowStates(states) == Qt::WindowMaximized)
        this->setWindowState(Qt::WindowMaximized);
    else {
        move(pos);
        resize(size);
    }
    setWindowFilePath(path);
}

void MainWindow::writeSettings()
{
    QSettings settings("NETGEN", "Netgen");

    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("path", windowFilePath());
    settings.setValue("state", int(windowState()));
    settings.setValue("recentFileList", recentFiles);
}

void MainWindow::setupRecentActions()
{
    for (int i = 0; i < maxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(slotOpenRecentFile()));
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::updateRecentFileActions(const QString &fileName)
{
    // Удаляем в меню старый список Recent-файлов
    for (int i = 0; i < recentFiles.size(); ++i)
        ui->menuRecentFiles->removeAction(recentFileActs[i]);

    // Модифицируем список Recent-файлов
    setWindowFilePath(fileName);
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    while (recentFiles.size() > maxRecentFiles)
        recentFiles.removeLast();
    createRecentMenu();
}

void MainWindow::createRecentMenu()
{
    // Создаем в меню новый список Recent-файлов
    for (int i = 0; i < recentFiles.size(); ++i) {
        QString text = QString("&%1 %2").arg(i + 1).arg(QFileInfo(recentFiles[i]).fileName());

        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(recentFiles[i]);
        recentFileActs[i]->setVisible(true);
        recentFileActs[i]->setStatusTip(recentFiles[i]);
        ui->menuRecentFiles->insertAction(nullptr, recentFileActs[i]);
    }
}

void MainWindow::slotOpenRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action)
        loadFile(action->data().toString());
    checkMenuState();
}

void MainWindow::slotGenerateMesh()
{
    if (modelType != ModelType::mesh)
        mesh = make_shared<Mesh>();
    unique_ptr<NgThread> thread = make_unique<NgThread>(mesh, geometry);

    isGenMeshStarted = true;
    isGenMeshCanceled = false;

    checkMenuState();

    setTerminate(0);

    startProgress(0, 100);
    thread->start();
    while (thread->isRunning())
        QCoreApplication::processEvents();
    stopProgress();
    isGenMeshStarted = false;

    if (!thread->isGenerated()) {
        if (isGenMeshCanceled)
            statusBar()->showMessage(tr("Process aborted by user!"), 5000);
        checkMenuState();
        return;
    }

    // Визуализация
    if (dynamic_cast<MeshView *>(centralWidget()) != nullptr)
        dynamic_cast<MeshView *>(centralWidget())->refresh(mesh.get());
    else
        setCentralWidget(new MeshView(mesh.get(), this));

    //setCentralWidget(new MeshView(mesh.get(), this));

    checkMenuState();
    cout << endl;

    statusBar()->showMessage(
        tr("Points: %1\t\t Elements: %2\t\t Surf Elements: %3")
            .arg(mesh->GetNP())
            .arg(mesh->GetNE())
            .arg(mesh->GetNSE()), 5000);

}

void MainWindow::slotStopMeshing()
{
    isGenMeshCanceled = true;
    setTerminate(1);
    checkMenuState();
}

void MainWindow::slotCSGOptions()
{
    unique_ptr<CSGSetupDialog> dlg = make_unique<CSGSetupDialog>(minX, maxX, &facets, &detail, this);

    if (dlg->exec() != QDialog::Accepted)
        return;
}

void MainWindow::slotMeshingOptions()
{
    unique_ptr<MeshingSetupDialog> dlg = make_unique<MeshingSetupDialog>(this);

    if (dlg->exec() != QDialog::Accepted)
        return;
}

void MainWindow::slotSave()
{
    bool rotating = static_cast<CSGView *>(centralWidget())
    ? static_cast<CSGView *>(centralWidget())->getRotating()
    : false;

    if (rotating)
        static_cast<CSGView *>(centralWidget())->setRotating(false);
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save"),
                                                    "",
                                                    tr("Mesh file (*.vol *vol.gz)"));
    if (rotating)
        static_cast<CSGView *>(centralWidget())->setRotating(true);

    if (fileName.length()) {
        if (!fileName.endsWith(".vol.gz", Qt::CaseInsensitive)
            && !fileName.endsWith(".vol", Qt::CaseInsensitive))
            fileName += ".vol.gz";
        mesh->Save(fileName.toStdString());
    }
}

void MainWindow::checkMenuState()
{
    // File
    ui->actionFileOpen->setEnabled(!isGenMeshStarted);
    ui->actionFileClose->setEnabled(geometry != nullptr || mesh != nullptr);
    ui->actionSaveMesh->setEnabled(mesh != nullptr && !isGenMeshStarted);
    ui->actionExportMesh->setEnabled(mesh != nullptr);
    ui->menuRecentFiles->setEnabled(!isGenMeshStarted);

    // Geometry
    ui->actionScanCSGGeometry->setEnabled((geometry != nullptr && modelType == ModelType::csg)
                                          && !isGenMeshStarted);
    ui->actionCSGOption->setEnabled((geometry != nullptr && modelType == ModelType::csg)
                                    && !isGenMeshStarted);

    // Mesh
    ui->actionStartMeshing->setEnabled((geometry != nullptr || mesh != nullptr)
                                       && !isGenMeshStarted);
    ui->actionStopMeshing->setEnabled(isGenMeshStarted);
    ui->actionRefineUniform->setEnabled(geometry != nullptr && mesh != nullptr);
    ui->actionMeshInfo->setEnabled(mesh != nullptr && !isGenMeshStarted);
    ui->actionMeshingOptions->setEnabled((mesh != nullptr || geometry != nullptr)
                                         && !isGenMeshStarted);

    // View
    ui->actionViewGeometry->setEnabled(geometry != nullptr);
    ui->actionViewMesh->setEnabled(mesh != nullptr);
    ui->actionViewTerminal->setChecked(dock->isVisible());
    ui->actionImageSetup->setEnabled(geometry != nullptr || mesh != nullptr);

    // Refinement
    // ui->actionRefineSecondOrder->setEnabled(isMeshCreated);
    // ui->actionRefineHighOrder->setEnabled(isMeshCreated);
    // ui->actionRefineOptions->setEnabled(mesh != nullptr);
}

void MainWindow::slotScanCSG()
{
    startProgress(0, 0);
    scanCSG(geometry, minX, maxX, detail, facets);
    stopProgress();

    setCentralWidget(new CSGView(geometry.get(), this));
}

void MainWindow::slotClose()
{
    modelType = ModelType::csg;
    geometry = nullptr;
    mesh = nullptr;
    setCentralWidget(new CSGView(nullptr, this));
    setWindowTitle("Netgen");
    checkMenuState();
}

void MainWindow::slotViewGeometry()
{
    if (dynamic_cast<MeshView *>(centralWidget()) != nullptr)
        setCentralWidget(modelType == ModelType::stl
                             ? new STLView(static_cast<STLGeometry *>(geometry.get()), this)
                             : new CSGView(static_cast<CSGeometry *>(geometry.get()), this));
}

void MainWindow::slotViewMesh()
{
    if (dynamic_cast<MeshView *>(centralWidget()) == nullptr)
        setCentralWidget(new MeshView(mesh.get(), this));
}

void MainWindow::slotViewTerminal()
{
    dock->setVisible(ui->actionViewTerminal->isChecked());
}

void MainWindow::slotExportMesh()
{
    QString selectedFilter;
    bool rotating = static_cast<CSGView *>(centralWidget())
                        ? static_cast<CSGView *>(centralWidget())->getRotating()
                        : false;

    if (rotating)
        static_cast<CSGView *>(centralWidget())->setRotating(false);
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Export"),
                                                    "",
                                                    tr("QFEM Mesh file (*.mesh)"),
                                                    &selectedFilter);
    if (rotating)
        static_cast<CSGView *>(centralWidget())->setRotating(true);

    if (fileName.length()) {
        QString extension;

        if (selectedFilter.contains("*.mesh"))
            extension = ".mesh";
        else
            extension = ".trpa";

        if (!extension.isEmpty() && !fileName.endsWith(extension, Qt::CaseInsensitive))
            fileName += extension;
        exportFile(fileName);
    }
}

void MainWindow::exportFile(const QString &fileName)
{
    bool ok{false};

    if (QFileInfo(fileName).suffix().toUpper() == "MESH")
        ok = exportMesh(fileName);

    if (ok) {
        statusBar()->showMessage(tr("File successfully exported"), 5000);
        //updateRecentFileActions(fileName);
    } else {
        statusBar()->showMessage(tr("Error exported file"), 5000);
        QMessageBox(QMessageBox::Critical, tr("Error"), tr("Error exported file")).exec();
    }
}

bool MainWindow::exportMesh(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);

    out << "fe3d4" << '\n';

    out << mesh->GetNP() << '\n';
    for (PointIndex pi = PointIndex::BASE; pi < mesh->GetNP() + PointIndex::BASE; pi++)
        out << (*mesh)[pi](0) << ' ' << (*mesh)[pi](1) << ' ' << (*mesh)[pi](2) << "\n";

    out << mesh->GetNE() << '\n';
    for (ElementIndex ei = 0; ei < mesh->GetNE(); ei++) {
        Element el = (*mesh)[ei];

        for (auto j = 0; j < el.GetNP(); j++)
            out << (el[j] - 1) << (j == el.GetNP() - 1 ? '\n' : ' ');
    }

    out << mesh->GetNSE() << '\n';

    for (SurfaceElementIndex sei = 0; sei < mesh->GetNSE(); sei++) {
        Element2d sel = (*mesh)[sei];

        for (auto j = 0; j < sel.GetNP(); j++)
            out << (sel[j] - 1) << (j == sel.GetNP() - 1 ? '\n' : ' ');
    }
    file.close();
    return true;
}

void MainWindow::slotRefineUniform()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    refinementMesh(geometry, mesh);
    if (dynamic_cast<MeshView *>(centralWidget()) != nullptr)
        dynamic_cast<MeshView *>(centralWidget())->refresh(mesh.get());
    else
        setCentralWidget(new MeshView(mesh.get(), this));
    QApplication::restoreOverrideCursor();
}

// void MainWindow::slotRefineSecondOrder()
// {
//     QApplication::setOverrideCursor(Qt::WaitCursor);
//     refineSecondOrder(geometry, mesh);
//     if (static_cast<CSGView*>(centralWidget())->type() == ModelType::mesh)
//         static_cast<MeshView*>(centralWidget())->refresh();
//     else
//         setCentralWidget(new MeshView(mesh.get(), this));
//     QApplication::restoreOverrideCursor();
// }

// void MainWindow::slotRefineHighOrder()
// {
//     int order = 10;

//     QApplication::setOverrideCursor(Qt::WaitCursor);
//     refineHighOrder(geometry, mesh, order);
//     if (static_cast<CSGView*>(centralWidget())->type() == ModelType::mesh)
//         static_cast<MeshView*>(centralWidget())->refresh();
//     else
//         setCentralWidget(new MeshView(mesh.get(), this));
//     QApplication::restoreOverrideCursor();
// }

// void MainWindow::slotRefineBisection()
// {
//     QApplication::setOverrideCursor(Qt::WaitCursor);
//     refineBisection(geometry, mesh);
//     if (static_cast<CSGView*>(centralWidget())->type() == ModelType::mesh)
//         static_cast<MeshView*>(centralWidget())->refresh();
//     else
//         setCentralWidget(new MeshView(mesh.get(), this));
//     QApplication::restoreOverrideCursor();

// }

void MainWindow::startProgress(int min, int max)
{
    pb->setMinimum(min);
    pb->setMaximum(max);
    pb->setValue(0);
    pb->show();
    if (max != 0)
        timer->start();
}

void MainWindow::stopProgress()
{
    timer->stop();
    pb->hide();
}

void MainWindow::slotImageSetup()
{
    ImageSetup(static_cast<CSGView *>(centralWidget())->getParams(),
               static_cast<CSGView *>(centralWidget()),
               this)
        .exec();
}

void MainWindow::slotMeshInfo()
{
    (new MeshInfo(mesh->GetNP(), mesh->GetNE(), mesh->GetNSE(), this))->exec();
}
