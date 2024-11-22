#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QStackedWidget>
#include <QStyle>
#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QTabBar> //for tabBar()
#include "widget/projecttab.h"
#include "window/about/aboutwindow.h"
#include "widget/ui_projecttab.h"

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	
	tabWidget = ui->tabWidget;
	
	//Remove welcome screen tab's close button
	tabWidget->tabBar()->setTabButton(0, QTabBar::LeftSide, 0);
	tabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, 0);
	
	ui->newProjectButton->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
	ui->newProjectButton->setIconSize(QSize(qApp->desktop()->logicalDpiX(), qApp->desktop()->logicalDpiY()));
	ui->openProjectButton->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
	ui->openProjectButton->setIconSize(QSize(qApp->desktop()->logicalDpiX(), qApp->desktop()->logicalDpiY()));
	
	connect(ui->newProjectButton, &QPushButton::clicked, this, &MainWindow::newProject);
	connect(ui->openProjectButton, &QPushButton::clicked, this, &MainWindow::openProject);
	
	// TODO add project handling
	newProject();
	tabWidget->removeTab(0);
	ui->actionNew_project->setVisible(false);
	ui->actionOpen_project->setVisible(false);
	ui->actionSave_project->setVisible(false);
	ui->actionClose_project->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::renameTab(const QString& name)
{
    tabWidget->setTabText(tabWidget->currentIndex(), name);
    setWindowTitle(tabWidget->tabText(tabWidget->currentIndex()) + " | GUIC - Graphical User-friendly Image Correlator");
}

void MainWindow::newProject()
{
    tabWidget->addTab(new ProjectTab(), "New project");
    tabWidget->setCurrentIndex(tabWidget->count()-1);
}

void MainWindow::openProject()
{
    QString filename = QFileDialog::getOpenFileName(nullptr, QObject::tr("Open project"), QDir::currentPath(), QObject::tr("GUIC projects (*.guic);;All files (*.*)"));
}

void MainWindow::closeWindow()
{
    for(int i = tabWidget->count()-1; i >= 0; i--)
    {
        if(!on_tabWidget_tabCloseRequested(i)) break;
    }
    QApplication::quit();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    // TODO add project handling
    //renameTab(tabWidget->tabText(tabWidget->currentIndex()));
}

bool MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if(tabWidget->currentIndex() == index)
    {
        if(index < tabWidget->count()-1)
        {
            tabWidget->setCurrentIndex(index+1);
        }
        else
        {
            tabWidget->setCurrentIndex(index-1);
        }
    }

    tabWidget->removeTab(index);
    return true;
}

void MainWindow::on_actionNew_project_triggered()
{
	newProject();
}

void MainWindow::on_actionOpen_project_triggered()
{
	openProject();
}

void MainWindow::on_actionAbout_GUIC_triggered()
{
	AboutWindow about;
	about.exec();
}

void MainWindow::on_actionQuit_GUIC_triggered()
{
    closeWindow();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    closeWindow();
}


void MainWindow::on_actionZoom_in_triggered()
{
    // TODO add project handling
    ProjectTab* tab = (ProjectTab*) tabWidget->currentWidget();
    tab->ui->scaleSpinBox->stepUp();
}


void MainWindow::on_actionZoom_out_triggered()
{
    // TODO add project handling
    ProjectTab* tab = (ProjectTab*) tabWidget->currentWidget();
    tab->ui->scaleSpinBox->stepDown();
}


void MainWindow::on_actionReset_zoom_triggered()
{
    // TODO add project handling
    ProjectTab* tab = (ProjectTab*) tabWidget->currentWidget();
    tab->ui->scaleSpinBox->setValue(100);
}


void MainWindow::on_actionOpen_images_triggered()
{
    // TODO add project handling
    ProjectTab* tab = (ProjectTab*) tabWidget->currentWidget();
    if(tab->ui->openImagesButton->isEnabled())
    {
        emit tab->ui->openImagesButton->released();
    }
}

