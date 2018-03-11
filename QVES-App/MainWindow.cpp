#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QSplitter>
#include <QTabWidget>
#include <QtCharts/QChartView>
#include <QFileDialog>
#include <QPixmap>
#include <QOpenGLWidget>
#include "NewCustomModelDialog.h"
#include <QSettings>
#include <QCoreApplication>
#include <QMessageBox>
#include <QDir>
#include "QVESSettingsDialog.h"

QT_CHARTS_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    recentProjectsMenu = new QMenu(tr("Proyectos recientes"), this);
    QAction* before = ui->menuFile->actions().at(2);
    ui->menuFile->insertMenu(before, recentProjectsMenu);

    rmsStatusLabel = new QLabel(this);
    ui->statusBar->addPermanentWidget(rmsStatusLabel);

    QSplitter *splitter = new QSplitter(this);
    QTabWidget *mainTabs = new QTabWidget(splitter);
    mDelegate = new QVESModelDelegate(this);
    mDataPanel = new DataPanel;
    mPropertiesPanel = new VESPropertiesPanel;
    mChart = new MainChart;

    createConnections();

    mDataPanel->setMaximumWidth(mDataPanel->sizeHint().width());
    mainTabs->addTab(mDataPanel, tr("Datos del SEV"));
    mPropertiesPanel->setMaximumWidth(mDataPanel->sizeHint().width());
    mainTabs->addTab(mPropertiesPanel, tr("Propiedades del SEV"));
    mainTabs->setMaximumWidth(mainTabs->sizeHint().width());



    chartView = new QChartView(mChart->chart);
    chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setFrameShape(QFrame::StyledPanel);
    chartView->setFrameShadow(QFrame::Raised);


    splitter->addWidget(mainTabs);
    splitter->addWidget(chartView);
    this->setCentralWidget(splitter);
    this->showMaximized();


    QCoreApplication::setOrganizationName("Joaco's Soft");
    QCoreApplication::setApplicationName("QVES");
    loadSettings();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createConnections()
{
    connect(mDataPanel, &DataPanel::currentVESIndexChanged, mDelegate, &QVESModelDelegate::selectedVESChanged);
    connect(mDataPanel, &DataPanel::currentVESModelIndexChanged, mDelegate, &QVESModelDelegate::changeCurrentModel);
    connect(mDataPanel, &DataPanel::rowSelectionChanged, mDelegate, &QVESModelDelegate::dataSelectionChanged);
    connect(mDelegate, &QVESModelDelegate::projectChanged, this, &MainWindow::loadProject);
    connect(mDelegate, &QVESModelDelegate::vesChanged, this, &MainWindow::loadVES);
    connect(mDelegate, &QVESModelDelegate::vesCurrentModelChanged, this, &MainWindow::modelChanged);
    //connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openProject);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveProject);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveAsProject);
    connect(ui->actionEmptyModel, &QAction::triggered, this, &MainWindow::createEmptyModel);
    connect(ui->actionZohdy, &QAction::triggered, mDelegate, &QVESModelDelegate::onZohdyInversionRequested);
    connect(mDataPanel, &DataPanel::showedDataChanged, mDelegate, &QVESModelDelegate::showedTableDataChanged);
    connect(mDelegate, &QVESModelDelegate::tableModelChanged, this, &MainWindow::modelUpdated);
    connect(ui->actionMergeBeds, &QAction::triggered, mDelegate, &QVESModelDelegate::mergeSelectedBeds);
    connect(ui->actionExportChart, &QAction::triggered, this, &MainWindow::exportChartAs);
    connect(this, &MainWindow::fieldVisibleChanged, mChart, &MainChart::setFieldVisible);
    connect(this, &MainWindow::spliceVisibleChanged, mChart, &MainChart::setSpliceVisible);
    connect(this, &MainWindow::calculatedVisibleChanged, mChart, &MainChart::setCalculatedVisible);
    connect(this, &MainWindow::modeledVisibleChanged, mChart, &MainChart::setModeledVisible);
}

void MainWindow::loadSettings()
{
    QSettings settings;

    mQVESSettings = new QVESSettings(this);
    connect(mQVESSettings, &QVESSettings::settingsLoaded, this, &MainWindow::onSettingsLoaded);
    mQVESSettings->setSettings(&settings);
    mQVESSettings->readSettings();

}

void MainWindow::writeSettings()
{
    QSettings settings;

    mQVESSettings->setSettings(&settings);
    mQVESSettings->writeSettings();
}

bool MainWindow::userWantsToSave() const
{
    bool result = true;
    return result;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
//    if (maybeSave()) {
//        writeSettings();
//        event->accept();
//    } else {
//        event->ignore();
//    }
}

void MainWindow::openProject(const QString filename)
{
    if (!filename.isNull()){
        mDelegate->openProject(filename);
        mQVESSettings->appendLastProject(filename);
    }
}

void MainWindow::loadProject()
{
    mDataPanel->loadVESNames(mDelegate->vesNames(), mDelegate->currentVESIndex());
    mLastDirectory = mDelegate->projectPath();
    mQVESSettings->setLastDirectory(mLastDirectory);
}

void MainWindow::loadVES()
{
    mDataPanel->setMyModel(mDelegate->currentModel());
    mChart->chartDelegateChanged(mDelegate);
    mDataPanel->loadModelNames(mDelegate->modelNames(), mDelegate->currentVESModelIndex());
    rmsStatusLabel->setText(mDelegate->modelError());
}

void MainWindow::modelChanged()
{
    rmsStatusLabel->setText(mDelegate->modelError());
}

void MainWindow::saveProject()
{
    if (mDelegate->projectFileName().isEmpty()){
        saveAsProject();
    } else {
        mDelegate->saveProject();
    }

}

void MainWindow::saveAsProject()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Guardar proyecto como..."),
                                                    mDelegate->projectPath(),
                                                    tr("Proyectos QVS (*.qvs)"));
    if (!fileName.isNull()){
        mDelegate->saveAsProject(fileName);
        mQVESSettings->appendLastProject(fileName);
    }
}

void MainWindow::modelUpdated()
{
    mDataPanel->setMyModel(mDelegate->currentModel());
}

void MainWindow::createEmptyModel()
{
    NewCustomModelDialog* diag = new NewCustomModelDialog(this);
    connect(diag, &NewCustomModelDialog::numberOfBedSelected, mDelegate, &QVESModelDelegate::createEmptyModel);

    diag->show();
}

void MainWindow::exportChartAs()
{
    QString proposed = QDir::cleanPath(mDelegate->projectPath() + QDir::separator() + mDelegate->vesName() + ".png");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Guardar gráfico como..."),
                                                    proposed,
                                                    tr("Imágenes PNG (*.png)"));
    if (!fileName.isNull()){
        QPixmap p = chartView->grab();
        QOpenGLWidget *glWidget  = chartView->findChild<QOpenGLWidget*>();
        if(glWidget){
            QPainter painter(&p);
            QPoint d = glWidget->mapToGlobal(QPoint())-chartView->mapToGlobal(QPoint());
            painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            painter.drawImage(d, glWidget->grabFramebuffer());
            painter.end();
        }
        p.save(fileName, "PNG");
    }
}

void MainWindow::on_actionShowFieldData_triggered()
{
    mQVESSettings->setShowField(ui->actionShowFieldData->isChecked());
    emit fieldVisibleChanged(ui->actionShowFieldData->isChecked());
}

void MainWindow::on_actionShowSplices_triggered()
{
    mQVESSettings->setShowSplice(ui->actionShowSplices->isChecked());
    emit spliceVisibleChanged(ui->actionShowSplices->isChecked());
}

void MainWindow::on_actionShowCalculatedData_triggered()
{
    mQVESSettings->setShowCalculated(ui->actionShowCalculatedData->isChecked());
    emit calculatedVisibleChanged(ui->actionShowCalculatedData->isChecked());
}

void MainWindow::on_actionShowModels_triggered()
{
    mQVESSettings->setShowModel(ui->actionShowModels->isChecked());
    emit modeledVisibleChanged(ui->actionShowModels->isChecked());
}

void MainWindow::onSettingsLoaded()
{
    mLastDirectory = mQVESSettings->lastDirectory();

    recentProjectsMenu->clear();
    foreach (auto &s, mQVESSettings->lastOpenedProjects()) {
        recentProjectsMenu->addAction(s, [=](){this->openProject(s);});
    }    

    ui->actionShowFieldData->setChecked(mQVESSettings->showField());
    emit fieldVisibleChanged(mQVESSettings->showField());

    ui->actionShowSplices->setChecked(mQVESSettings->showSplice());
    emit spliceVisibleChanged(mQVESSettings->showSplice());

    ui->actionShowCalculatedData->setChecked(mQVESSettings->showCalculated());
    emit calculatedVisibleChanged(mQVESSettings->showCalculated());

    ui->actionShowModels->setChecked(mQVESSettings->showModel());
    emit modeledVisibleChanged(mQVESSettings->showModel());

    mDelegate->setZohdyFilter(mQVESSettings->zohdyFilter());
    mDelegate->setAutoDarZarrouk(mQVESSettings->autoDarZarrouk());
    mDelegate->setAutoDarZarroukThreshold(mQVESSettings->autoDarZarroukThreshold());
    mDelegate->setVFSAInitialTemperature(mQVESSettings->vFSAInitialTemperature());
    mDelegate->setVFSAIterationsPerTemperature(mQVESSettings->vFSAIterationsPerTemperature());
    mDelegate->setVFSAMaximunError(mQVESSettings->vFSAMaximunError());
    mDelegate->setVFSAMinimunPdf(mQVESSettings->vFSAMinimunPdf());
    mDelegate->setVFSAMovesPerTemperature(mQVESSettings->vFSAMovesPerTemperature());
    mDelegate->setVFSANumberOfBeds(mQVESSettings->vFSANumberOfBeds());
    mDelegate->setVFSASolutions(mQVESSettings->vFSASolutions());

    mChart->loadQVESSettings(mQVESSettings);
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Abrir proyecto..."),
                                                    mLastDirectory,
                                                    tr("Proyectos QVS (*.qvs);; Proyectos antiguos de SEVs (*.sev)"));
    openProject(fileName);
}

void MainWindow::on_actionInversionOptions_triggered()
{
    QVESSettingsDialog* diag = new QVESSettingsDialog(mQVESSettings, 1, this);
    diag->exec();
}

void MainWindow::on_actionChartOptions_triggered()
{
    QVESSettingsDialog diag(mQVESSettings, 2, this);
    diag.exec();
}

void MainWindow::on_actionGeneralOptions_triggered()
{
    QVESSettingsDialog diag(mQVESSettings, 0, this);
    diag.exec();
}
