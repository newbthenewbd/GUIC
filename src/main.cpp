#include "main.h"
#include <QApplication>
#include <QStackedWidget>
#include "mainwindow.h"

int main(int argc, char** argv) {
	
    QApplication app(argc, argv);

	MainWindow mainWindow;
	
	//mainWindow.setStatusBar(NULL);
	mainWindow.showMaximized();
	
	//mainWindow.stackedWidget;
	
	//mainWindowViews = (QStackedWidget*) window.centralWidget()->children()[0];
	
	//mainWindowViews->addWidget(welcomeView);
	//mainWindowViews->addWidget(viewImageView);
	
	//mainWindowViews->setCurrentWidget(welcomeView);
	
    return app.exec();
	
}