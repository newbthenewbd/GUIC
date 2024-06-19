#ifndef PROJECTTAB_H
#define PROJECTTAB_H

#define W_NO_PROPERTY_MACRO
#include <wobjectdefs.h>
#include <QObject>
#include <QEvent>
#include <QWidget>
#include <QStringList>
#include <QListWidgetItem>
#include <QAction>
#include <QPainter>
#include <QGraphicsScene>

typedef enum {
    UNITS_PX_PERCENT, UNITS_MM_PERCENT, /*first non-unit menu item*/ UNITS_CALIBRATE, UNITS_MAX
} UnitsActionID;
static QString unitsUnits[][UNITS_CALIBRATE] = {{"px", "%"}, {"mm", "%"}};

typedef enum {
    UNIT_TYPE_DEFORMATION, UNIT_TYPE_STRAIN, UNIT_TYPE_MAX
} UnitTypeID;

typedef enum {
    SOLVER_FFTCC_NR, SOLVER_MAX
} SolverActionID;

namespace Ui {
    class ProjectTab;
}

class ProjectTab : public QWidget
{
    //Q_OBJECT
    W_OBJECT(ProjectTab)
    
    private:
    Ui::ProjectTab* ui;
    QPainter painter;
    QGraphicsScene* scene;
    QMenu* unitsMenu;
    UnitsActionID unitsAction = UNITS_PX_PERCENT;
    QAction* unitsActions[UNITS_MAX];
    QMenu* solverMenu;
    SolverActionID solverAction = SOLVER_FFTCC_NR;
    QAction* solverActions[SOLVER_MAX];
    double pxCalibrated = 0.0, mmCalibrated = 0.0, mmPerPxFactor = 1.0;
    
    protected:
    bool eventFilter(QObject* object, QEvent* event);
    
    public:
    explicit ProjectTab(QWidget* parent = nullptr);
    ~ProjectTab();
    
    public slots:
    void addImages();
    W_SLOT(addImages)
    void addImagesFromPaths(QStringList paths);
    W_SLOT(addImagesFromPaths)
    void imageListItemSelected(QListWidgetItem* imageListItem, QListWidgetItem* prevImageListItem);
    W_SLOT(imageListItemSelected, (QListWidgetItem*, QListWidgetItem*))
    void displaySelected(int displayId);
    W_SLOT(displaySelected)
    void colormapSelected(int colormapId);
    W_SLOT(colormapSelected)
    void solverChanged(QAction* action);
    W_SLOT(solverChanged, (QAction*))
    void unitsChanged(QAction* action);
    W_SLOT(unitsChanged, (QAction*))
    void solve();
    W_SLOT(solve)
};

#endif // PROJECTTAB_H
