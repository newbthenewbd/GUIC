#include "solverconfigwindow.h"
#include "ui_solverconfigwindow.h"

SolverConfigWindow::SolverConfigWindow(ProjectTab* parent) :
QDialog(parent),
ui(new Ui::SolverConfigWindow)
{
    ui->setupUi(this);
    ui->uniformPOIDistance->setValue((int) parent->uniformPOIDistance);
    ui->initSubsetRadiusX->setValue(parent->initSubsetRadiusX);
	ui->initSubsetRadiusY->setValue(parent->initSubsetRadiusY);
	ui->iterSubsetRadiusX->setValue(parent->iterSubsetRadiusX);
	ui->iterSubsetRadiusY->setValue(parent->iterSubsetRadiusY);
	ui->maxDeformationNorm->setValue((double) parent->maxDeformationNorm);
	ui->maxIter->setValue(parent->maxIter);
	ui->strainRadius->setValue((double) parent->strainRadius);
	ui->minStrainNeighbors->setValue(parent->minStrainNeighbors);
}

SolverConfigWindow::~SolverConfigWindow()
{
    delete ui;
}

void SolverConfigWindow::on_okButton_clicked()
{
	accept();
}

void SolverConfigWindow::on_resetButton_clicked()
{
	// TODO DRY with projecttab.h
	ui->initSubsetRadiusX->setValue(16);
	ui->initSubsetRadiusY->setValue(16);
	ui->iterSubsetRadiusX->setValue(16);
	ui->iterSubsetRadiusY->setValue(16);
	ui->maxIter->setValue(16);
	ui->maxDeformationNorm->setValue(0.001);
	ui->strainRadius->setValue(20.0);
	ui->minStrainNeighbors->setValue(4);
	ui->uniformPOIDistance->setValue(8);
}

void SolverConfigWindow::on_uniformPOIDistance_valueChanged(int arg1)
{
	((ProjectTab*) parent())->uniformPOIDistance = (unsigned) arg1;
}

void SolverConfigWindow::on_initSubsetRadiusX_valueChanged(int arg1)
{
	((ProjectTab*) parent())->initSubsetRadiusX = arg1;
}

void SolverConfigWindow::on_initSubsetRadiusY_valueChanged(int arg1)
{
	((ProjectTab*) parent())->initSubsetRadiusY = arg1;
}

void SolverConfigWindow::on_iterSubsetRadiusX_valueChanged(int arg1)
{
	((ProjectTab*) parent())->iterSubsetRadiusX = arg1;
}

void SolverConfigWindow::on_iterSubsetRadiusY_valueChanged(int arg1)
{
	((ProjectTab*) parent())->iterSubsetRadiusY = arg1;
}

void SolverConfigWindow::on_maxDeformationNorm_valueChanged(double arg1)
{
	((ProjectTab*) parent())->maxDeformationNorm = (float) arg1;
}

void SolverConfigWindow::on_maxIter_valueChanged(int arg1)
{
	((ProjectTab*) parent())->maxIter = arg1;
}

void SolverConfigWindow::on_strainRadius_valueChanged(double arg1)
{
	((ProjectTab*) parent())->strainRadius = (float) arg1;
}

void SolverConfigWindow::on_minStrainNeighbors_valueChanged(int arg1)
{
	((ProjectTab*) parent())->minStrainNeighbors = arg1;
}

