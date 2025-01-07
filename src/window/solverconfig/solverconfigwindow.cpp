#include "solverconfigwindow.h"
#include "ui_solverconfigwindow.h"

SolverConfigWindow::SolverConfigWindow(ProjectTab* parent) :
QDialog(parent),
ui(new Ui::SolverConfigWindow)
{
    ui->setupUi(this);
    ui->uniformPOIDistance->setValue((int) parent->uniformPOIDistance);
	ui->subsetRadiusX->setValue(parent->subsetRadiusX);
	ui->subsetRadiusY->setValue(parent->subsetRadiusY);
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
	ui->subsetRadiusX->setValue(16);
	ui->subsetRadiusY->setValue(16);
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

void SolverConfigWindow::on_subsetRadiusX_valueChanged(int arg1)
{
	((ProjectTab*) parent())->subsetRadiusX = arg1;
}

void SolverConfigWindow::on_subsetRadiusY_valueChanged(int arg1)
{
	((ProjectTab*) parent())->subsetRadiusY = arg1;
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

