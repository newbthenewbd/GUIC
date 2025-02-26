#ifndef SOLVERCONFIGWINDOW_H
#define SOLVERCONFIGWINDOW_H

#include <QDialog>
#include "window/main/widget/projecttab.h"

namespace Ui {
    class SolverConfigWindow;
}

class SolverConfigWindow : public QDialog
{
    Q_OBJECT
    
    public:
    explicit SolverConfigWindow(ProjectTab* parent);
    ~SolverConfigWindow();
    
    private slots:
    void on_okButton_clicked();
    
    void on_resetButton_clicked();
    
    void on_uniformPOIDistance_valueChanged(int arg1);
    
    void on_initSubsetRadiusX_valueChanged(int arg1);
    
    void on_initSubsetRadiusY_valueChanged(int arg1);
    
    void on_iterSubsetRadiusX_valueChanged(int arg1);
    
    void on_iterSubsetRadiusY_valueChanged(int arg1);
    
    void on_maxDeformationNorm_valueChanged(double arg1);
    
    void on_maxIter_valueChanged(int arg1);
    
    void on_strainRadius_valueChanged(double arg1);
    
    void on_minStrainNeighbors_valueChanged(int arg1);
    
    private:
    Ui::SolverConfigWindow *ui;
};

#endif // SOLVERCONFIGWINDOW_H
