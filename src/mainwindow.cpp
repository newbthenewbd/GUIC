#include "mainwindow.h"
#include "ui_mainwindow.h"
#define W_NO_PROPERTY_MACRO
#include <wobjectimpl.h>
#include <QApplication>
#include <QStackedWidget>
#include <QStyle>
#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QTabBar> //for tabBar()
#include "projecttab.h"

W_OBJECT_IMPL(MainWindow)

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	
	tabWidget = ui->tabWidget;
	
	//Remove welcome screen tab's close button
	tabWidget->tabBar()->setTabButton(0, QTabBar::LeftSide, 0);
	tabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, 0);
	
	//connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));
	//connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
	connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::changeTab);
	connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
	
	ui->newProjectButton->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
	ui->newProjectButton->setIconSize(QSize(qApp->desktop()->logicalDpiX(), qApp->desktop()->logicalDpiY()));
	ui->openProjectButton->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
	ui->openProjectButton->setIconSize(QSize(qApp->desktop()->logicalDpiX(), qApp->desktop()->logicalDpiY()));
	
	connect(ui->newProjectButton, &QPushButton::clicked, this, &MainWindow::newProject);
	connect(ui->openProjectButton, &QPushButton::clicked, this, &MainWindow::openProject);
}

void MainWindow::changeTab(int index)
{
	
}

void MainWindow::closeTab(int index)
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
}

void MainWindow::newProject()
{
	tabWidget->addTab(new ProjectTab(), "New Project");
	tabWidget->setCurrentIndex(tabWidget->count()-1);
}

void MainWindow::openProject()
{
	QString filename = QFileDialog::getOpenFileName(nullptr, QObject::tr("Open Project"), QDir::currentPath(), QObject::tr("OpenCorrGUI files (*.ocg);;All files (*.*)"));
}

MainWindow::~MainWindow()
{
	delete ui;
}
