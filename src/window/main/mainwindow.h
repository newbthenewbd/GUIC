#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QTabWidget>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
	public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();
	void closeEvent(QCloseEvent* event);
	QTabWidget* tabWidget;
	
	public slots:
	void changeTab(int index);
	void closeTab(int index);
	void newProject();
	void openProject();
	
	private slots:
	void on_actionOpen_Project_triggered();
	
	private slots:
	void on_actionNew_Project_triggered();
	
	private slots:
	void on_actionAbout_GUIC_triggered();
	
	private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
