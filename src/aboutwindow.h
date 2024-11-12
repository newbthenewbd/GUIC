#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#define W_NO_PROPERTY_MACRO
#include <wobjectdefs.h>
#include <QDialog>

namespace Ui {
    class AboutWindow;
}

class AboutWindow : public QDialog
{
    //Q_OBJECT
    W_OBJECT(AboutWindow)
    
    public:
    explicit AboutWindow(QWidget *parent = nullptr);
    ~AboutWindow();
    
    private slots:
    void on_pushButton_clicked();
    W_SLOT(on_pushButton_clicked)
    
    private:
    Ui::AboutWindow *ui;
};

#endif // ABOUTWINDOW_H
