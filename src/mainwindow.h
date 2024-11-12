#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define W_NO_PROPERTY_MACRO
#include <wobjectdefs.h>
#include <QMainWindow>
#include <QTabWidget>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	//Q_OBJECT
	W_OBJECT(MainWindow)
	
	public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();
	QTabWidget* tabWidget;
	
	public slots:
	void changeTab(int index);
	void closeTab(int index);
	void newProject();
	void openProject();
	
	private slots:
	void on_actionAbout_GUIC_triggered();
	W_SLOT(on_actionAbout_GUIC_triggered)
	
	private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
