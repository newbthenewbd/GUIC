#include "main.h"
#include <QApplication>
#include <QStackedWidget>
#include "window/main/mainwindow.h"

// Manually include the GUI plugins for static builds
#include <qconfig.h>
#ifdef QT_STATIC

#include <QtPlugin>
#if _WIN32
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#elif __APPLE__
Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin)
#else
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
#endif

#endif

#if __APPLE__
#include <objc/runtime.h>
#include <objc/message.h>
#endif

int main(int argc, char** argv)
{
	// Get rid of MacOS duplicated tabs
	// See https://stackoverflow.com/q/39399553 (accessed 2024/11/12; thanks!)
#if __APPLE__
	((id (*)(Class, SEL, bool)) objc_msgSend)(objc_getClass("NSWindow"), sel_getUid("setAllowsAutomaticWindowTabbing:"), false);
#endif
	// ...yes, I'm a C programmer, why do you ask?
	
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	
	QApplication app(argc, argv);
	
	QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
	
	app.setStyle("Fusion");

	MainWindow mainWindow;

	mainWindow.showMaximized();
	
	return app.exec();
}
