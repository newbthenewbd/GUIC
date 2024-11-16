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
	void newProject();
	void openProject();
    // void saveProject();
    // void closeProject();
    // void closeWindow();

	private slots:
    void on_tabWidget_currentChanged(int index);
    void on_tabWidget_tabCloseRequested(int index);
	void on_actionOpen_Project_triggered();
	void on_actionNew_Project_triggered();
    void on_actionAbout_GUIC_triggered();

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
