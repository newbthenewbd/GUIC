#include "aboutwindow.h"
#define W_NO_PROPERTY_MACRO
#include <wobjectimpl.h>
#include "ui_aboutwindow.h"

W_OBJECT_IMPL(AboutWindow)

AboutWindow::AboutWindow(QWidget *parent) :
QDialog(parent),
ui(new Ui::AboutWindow)
{
	ui->setupUi(this);
}

AboutWindow::~AboutWindow()
{
	delete ui;
}

void AboutWindow::on_pushButton_clicked()
{
	accept();
}

