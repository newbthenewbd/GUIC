#include "main.h"

#include <qconfig.h>
#if QT_STATIC

#include <QtPlugin>
#if _WIN32
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#elif __APPLE__
Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin)
#else
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
#endif

#endif

#include <QApplication>
#include <QStackedWidget>
#include "mainwindow.h"

int main(int argc, char** argv) {
	
    QApplication app(argc, argv);
    
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	MainWindow mainWindow;

	mainWindow.showMaximized();
	
    return app.exec();
	
}