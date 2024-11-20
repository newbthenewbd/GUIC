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
}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::on_actionNew_Project_triggered()
{
	newProject();
}

void MainWindow::on_actionOpen_Project_triggered()
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

