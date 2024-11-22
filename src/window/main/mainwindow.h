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
	void renameTab(const QString&);
	void newProject();
	void openProject();
    // void saveProject();
    // void closeProject();
    void closeWindow();

	private slots:
    void on_tabWidget_currentChanged(int index);
    bool on_tabWidget_tabCloseRequested(int index);
	void on_actionOpen_project_triggered();
	void on_actionNew_project_triggered();
	void on_actionAbout_GUIC_triggered();
	void on_actionQuit_GUIC_triggered();
    
    void on_actionZoom_in_triggered();
    
    void on_actionZoom_out_triggered();
    
    void on_actionReset_zoom_triggered();
    
    void on_actionOpen_images_triggered();
    
    private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
