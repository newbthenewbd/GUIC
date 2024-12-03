#ifndef PROJECTTAB_H
#define PROJECTTAB_H

#include <QObject>
#include <QEvent>
#include <QWidget>
#include <QStringList>
#include <QListWidgetItem>
#include <QAction>
#include <QPainter>
#include <QGraphicsScene>
#include <memory>
#include <opencorr.h>

typedef enum {
	UNITS_PX_PERCENT, UNITS_MM_PERCENT, UNITS_CALIBRATE, UNITS_MANCALIBRATE, UNITS_MAX
} UnitsActionID;

typedef enum {
	UNIT_TYPE_DEFORMATION, UNIT_TYPE_CAUCHY_STRAIN, UNIT_TYPE_GREEN_STRAIN, UNIT_TYPE_MAX
} UnitTypeID;
static QString unitsUnits[][UNIT_TYPE_MAX] = {
	// UNITS_PX_PERCENT	
	{
		"px", // UNIT_TYPE_DEFORMATION
		"%", // UNIT_TYPE_CAUCHY_STRAIN
		"%" // UNIT_TYPE_GREEN_STRAIN
	},
	// UNITS_MM_PERCENT
	{
		"mm", // UNIT_TYPE_DEFORMATION
		"%", // UNIT_TYPE_CAUCHY_STRAIN
		"%" // UNIT_TYPE_GREEN_STRAIN
	}
};

typedef enum {
	SOLVER_FFTCC_ICGN1, SOLVER_FFTCC_ICGN2, SOLVER_FFTCC_NR1, SOLVER_CONFIGURE, SOLVER_MAX
} SolverActionID;

typedef enum {
	MODE_NORMAL, MODE_TWOPOINTS_POINT1, MODE_TWOPOINTS_POINT2, MODE_POLYGON
} SceneViewMode;

typedef struct {
	UnitTypeID unit;
	int subUnit;
} DisplayType;

namespace Ui {
	class ProjectTab;
}

class ProjectTab : public QWidget
{
	Q_OBJECT
	
	
	private slots:
	void on_maxSpinBox_valueChanged(double value);
	
	void on_minSpinBox_valueChanged(double value);
	
	void on_aoiButton_released();
	
	public:
	explicit ProjectTab(QWidget* parent = nullptr);
	~ProjectTab();
	
	// Solver config
	int subsetRadiusX = 16;
	int subsetRadiusY = 16;
	int maxIter = 10;
	float maxDeformationNorm = 0.001f;
	float strainRadius = 20.f;
	int minStrainNeighbors = 5;
	unsigned uniformPOIDistance = 8;
	
	Ui::ProjectTab* ui;
	QPainter painter;
	QGraphicsScene* scene;
	QMenu* unitsMenu;
	QActionGroup* unitsActionGroup;
	UnitsActionID unitsAction = UNITS_PX_PERCENT;
	QAction* unitsActions[UNITS_MAX];
	QMenu* solverMenu;
	QActionGroup* solverActionGroup;
	SolverActionID solverAction = SOLVER_FFTCC_NR1;
	QAction* solverActions[SOLVER_MAX];
	SceneViewMode sceneViewMode = MODE_NORMAL;
	QPointF sceneViewPoint1, sceneViewPoint2;
	QPolygonF sceneViewPolygon;
	double pxCalibrated = 1.0, mmCalibrated = 1.0, mmPerPxFactor = 1.0;
	std::vector<DisplayType> displayTypes = { // TODO make global, editable
		(DisplayType) {UNIT_TYPE_MAX, 0}, // null
		(DisplayType) {UNIT_TYPE_MAX, 1}, // Convergence (at origin)
		(DisplayType) {UNIT_TYPE_MAX, 2}, // Convergence (deformed)
		(DisplayType) {UNIT_TYPE_DEFORMATION, 0},
		(DisplayType) {UNIT_TYPE_DEFORMATION, 1},
		(DisplayType) {UNIT_TYPE_DEFORMATION, 2},
		(DisplayType) {UNIT_TYPE_CAUCHY_STRAIN, 0},
		(DisplayType) {UNIT_TYPE_CAUCHY_STRAIN, 1},
		(DisplayType) {UNIT_TYPE_CAUCHY_STRAIN, 2},
		(DisplayType) {UNIT_TYPE_GREEN_STRAIN, 0},
		(DisplayType) {UNIT_TYPE_GREEN_STRAIN, 1},
		(DisplayType) {UNIT_TYPE_GREEN_STRAIN, 2}
	};
	DisplayType displayType = (DisplayType) {UNIT_TYPE_MAX, 0};
	bool customMin = false, customMax = false;
	std::unique_ptr<opencorr::FFTCC2D> fftccSolver;
	std::unique_ptr<opencorr::DIC> dicSolver;
	std::unique_ptr<opencorr::Strain> strainSolver;
	
	protected:
	bool eventFilter(QObject* object, QEvent* event);
	
	public slots:
	void addImages();
	void addImagesFromPaths(QStringList paths);
	void displayImage();
	void displaySelected(int displayId);
	void colormapSelected(int colormapId);
	void solverChanged(QAction* action);
	void unitsChanged(QAction* action);
	void resetMax();
	void resetMin();
	void solve();
};

#endif // PROJECTTAB_H
