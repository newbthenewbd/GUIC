#include "projecttab.h"
#include "ui_projecttab.h"
#include <QObject>
#include <QEvent>
#include <QGraphicsSceneEvent>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPixmap>
#include <QImage>
#include <QStringList>
#include <QStyleFactory>
#include <QMenu>
#include <QAction>
#include <QComboBox>
#include <QInputDialog>
#include <QMessageBox>
#include "window/solverconfig/solverconfigwindow.h"
#include "imagelistitemwidget.h"
#include "model/imagelistitemdata.h"
#include <omp.h>
#include <opencv2/opencv.hpp>
//#include <opencv2/world.hpp>
#define TINYCOLORMAP_WITH_QT5
#include "ext/tinycolormap.hpp"
#include <vector>

ProjectTab::ProjectTab(QWidget* parent) :
QWidget(parent),
ui(new Ui::ProjectTab)
{
	ui->setupUi(this);
	
	// Makes rendering EVEN SLOWER!
	/*QOpenGLWidget* glWidget = new QOpenGLWidget();
	ui->sceneView->setViewport(glWidget);
	ui->sceneView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);*/
	
	scene = new QGraphicsScene();
	ui->sceneView->setScene(scene);
	
	colormapSelected(0);
	
    QStyle* fusion = QStyleFactory::create("Fusion"); //pointer ownership belongs to Qt
	
	ui->displayConfigButton->setStyle(fusion);
    ui->displayConfigButton->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
	
	for(QObject* object : ui->toolBar->children())
	{
		if(object->isWidgetType()) ((QWidget*) object)->setStyle(fusion);
	}
	
    ui->saveProjectButton->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
	
    ui->openImagesButton->setIcon(style()->standardIcon(QStyle::SP_DirHomeIcon));
	
	ui->solverButton->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
	solverMenu = new QMenu();
	solverActions[SOLVER_FFTCC_ICGN1] = new QAction("FFTCC + ICGN1", nullptr);
	solverActions[SOLVER_FFTCC_ICGN2] = new QAction("FFTCC + ICGN2", nullptr);
	solverActions[SOLVER_FFTCC_NR1] = new QAction("FFTCC + NR1", nullptr);
	solverActions[SOLVER_CONFIGURE] = new QAction("Configure solvers...", nullptr);
	solverActionGroup = new QActionGroup(nullptr);
	for(int i = 0; i < SOLVER_CONFIGURE; i++)
	{
		solverActions[i]->setCheckable(true);
		if(i == 0) solverActions[i]->setChecked(true);
		solverActionGroup->addAction(solverActions[i]);
	}
	for(int i = 0; i < SOLVER_MAX; i++)
	{
		if(i == SOLVER_CONFIGURE)
		{
			solverMenu->addSeparator();
		}
		solverMenu->addAction(solverActions[i]);
	}
	ui->solverButton->setMenu(solverMenu);
	
	ui->unitsButton->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
	unitsMenu = new QMenu();
	unitsActions[UNITS_PX_PERCENT] = new QAction("Display units: px, %", nullptr);
	unitsActions[UNITS_MM_PERCENT] = new QAction("Display units: mm, %", nullptr);
	unitsActions[UNITS_CALIBRATE] = new QAction("Calibrate units...", nullptr);
	unitsActions[UNITS_MANCALIBRATE] = new QAction("Calibrate manually... [1 px = 1 mm]", nullptr);
	unitsActionGroup = new QActionGroup(nullptr);
	for(int i = 0; i < UNITS_CALIBRATE; i++)
	{
		unitsActions[i]->setCheckable(true);
		if(i == 0) unitsActions[i]->setChecked(true);
		unitsActionGroup->addAction(unitsActions[i]);
	}
	for(int i = 0; i < UNITS_MAX; i++)
	{
		if(i == UNITS_CALIBRATE)
		{
			unitsMenu->addSeparator();
		}
		unitsMenu->addAction(unitsActions[i]);
	}
	ui->unitsButton->setMenu(unitsMenu);
	
	ui->aoiButton->setIcon(style()->standardIcon(QStyle::SP_FileDialogListView));
	
	ui->solveButton->setIcon(style()->standardIcon(QStyle::SP_DialogOkButton));
	
	// Workaround UI bug wherein the label space remains at the beginning even though no unit is shown
	ui->colormap->setUnit("units");
	ui->colormap->setUnit("");
	
	// TODO add project handling
	ui->saveProjectButton->setVisible(false);
	
	// TODO add display config
	ui->displayConfigButton->setVisible(false);
	
	this->installEventFilter(this);
	scene->installEventFilter(this);
}

void ProjectTab::addImages()
{
	QStringList paths = QFileDialog::getOpenFileNames(this, QObject::tr("Add images"), "" /*TODO remember last directory*/, QObject::tr("Images (*.bmp *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.tif *.tiff *.xbm *.xpm);;BMP (*.bmp);;JPEG (*.jpg *.jpeg);;PNG (*.png);;PBM (*.pbm);;PGM (*.pgm);;PPM (*.ppm);;TIFF (*.tif *.tiff);;XBM (*.xbm);;XPM (*.xpm);;All files (*.*)")
	/*#ifdef Q_OS_MAC
	, nullptr, QFileDialog::DontUseNativeDialog //these file dialogs are the worst in the world!!!
	#endif*/
	);
	addImagesFromPaths(paths);
}

void ProjectTab::addImagesFromPaths(QStringList paths)
{
	ui->openImagesButton->setEnabled(false);
	ui->aoiButton->setEnabled(false);
	ui->solverButton->setEnabled(false);
	ui->solveButton->setEnabled(false);
	ui->unitsButton->setEnabled(false);
	for(QStringList::iterator i = paths.begin(); i != paths.end(); i++)
	{
		QString path = *i;
		QFileInfo fileInfo(*i);
		ui->statusLabel->setText("Loading " + fileInfo.fileName() + "...");
		qApp->processEvents(); //maintain responsiveness
		
		try {
			ImageListItemWidget* item = new ImageListItemWidget();
			item->setText(fileInfo.completeBaseName());
			
			ImageListItemData* data = new ImageListItemData();
			item->setData(data);
			
			data->image = std::make_unique<opencorr::Image2D>(path.toStdString());
			
			data->pixmap = QPixmap::fromImage(QImage(data->image.get()->cv_mat.data, data->image.get()->width, data->image.get()->height, data->image.get()->width, QImage::Format_Grayscale8));
			
			item->setIcon(data->pixmap);
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
			ui->listWidget->addItem(item);
			qApp->processEvents(); //maintain responsiveness
		} catch(...) {
			QMessageBox::critical(nullptr, "Error", "Could not load " + fileInfo.fileName());
			break;
		}
	}
	ui->openImagesButton->setEnabled(true);
	ui->aoiButton->setEnabled(true);
	ui->solverButton->setEnabled(true);
	ui->solveButton->setEnabled(true);
	ui->unitsButton->setEnabled(true);
	ui->statusLabel->setText("Ready");
}

void ProjectTab::displaySelected(int displayId)
{
	displayType = displayTypes[displayId];
	
	if(displayType.unit == UNIT_TYPE_MAX && displayType.subUnit != 0)
	{
		ui->minSpinBox->setDecimals(7);
		ui->maxSpinBox->setDecimals(7);
		ui->colormap->setDecimals(7);
	}
	else if(ui->minSpinBox->decimals() != 3)
	{
		ui->minSpinBox->setDecimals(3);
		ui->maxSpinBox->setDecimals(3);
		ui->colormap->setDecimals(3);
	}
	
	if(displayType.unit == UNIT_TYPE_MAX)
	{
		if(ui->colormap->getUnit() != "" || displayType.subUnit == 0)
		{
			customMin = false;
			customMax = false;
		}
		ui->colormap->setUnit("");
	}
	else
	{
		ui->colormap->setUnit(unitsUnits[unitsAction][displayType.unit]);
		customMin = false;
		customMax = false;
	}
	displayImage();
}

void ProjectTab::colormapSelected(int colormapId)
{
	tinycolormap::ColormapType colormapBoxTypes[] = { // TODO refactor out... along with the whole selector
		tinycolormap::ColormapType::Heat,
		tinycolormap::ColormapType::Turbo,
		tinycolormap::ColormapType::Jet,
		tinycolormap::ColormapType::Parula,
		tinycolormap::ColormapType::Hot,
		tinycolormap::ColormapType::Gray,
		tinycolormap::ColormapType::Magma,
		tinycolormap::ColormapType::Inferno,
		tinycolormap::ColormapType::Plasma,
		tinycolormap::ColormapType::Viridis,
		tinycolormap::ColormapType::Cividis,
		tinycolormap::ColormapType::Cubehelix,
		tinycolormap::ColormapType::HSV
	};
	ui->colormap->setColormap(colormapBoxTypes[colormapId]);
	ui->belowMinColorButton->setColor(ui->colormap->getColor(0.0));
	ui->aboveMaxColorButton->setColor(ui->colormap->getColor(1.0));
	displayImage();
}

void ProjectTab::solverChanged(QAction* action)
{
	if(action == solverActions[SOLVER_CONFIGURE])
	{
		SolverConfigWindow solverConfig(this);
		solverConfig.exec();
	}
	else
	{
		for(int i = 0; i < SOLVER_MAX; i++) //should always find one, if not found, do nothing at all
		{
			if(solverActions[i] == action)
			{
				ui->solverButton->setText("Solver: " + action->text() + " ");
				solverAction = (SolverActionID) i;
				//TODO recalculate
				break;
			}
		}
	}
}

void ProjectTab::unitsChanged(QAction* action)
{
	//QCoreApplication::postEvent(ui->unitsButton, new QHoverEvent(QEvent::HoverLeave, QPointF(), QPointF())); // no bueno, doesn't unhover the button still
	
	if(action == unitsActions[UNITS_CALIBRATE])
	{
		ui->statusLabel->setText("Click to grab coordinate 1, or Esc to cancel");
		ui->sceneView->setCursor(Qt::CrossCursor);
		sceneViewMode = MODE_TWOPOINTS_POINT1;
		ui->openImagesButton->setEnabled(false);
		ui->aoiButton->setEnabled(false);
		ui->solverButton->setEnabled(false);
		ui->solveButton->setEnabled(false);
		ui->unitsButton->setEnabled(false);
	}
	else if(action == unitsActions[UNITS_MANCALIBRATE])
	{
		bool got;
		
		double px = QInputDialog::getDouble(this, "Calibrate units...", "Enter dimension in pixels:", pxCalibrated, DBL_MIN, DBL_MAX, 15, &got);
		if(!got) return;
		
		double mm = QInputDialog::getDouble(this, "Calibrate units...", "Enter dimension in millimeters:", mmCalibrated, DBL_MIN, DBL_MAX, 15, &got);
		if(!got) return;
		
		pxCalibrated = px;
		mmCalibrated = mm;
		mmPerPxFactor = mm/px;
		unitsActions[UNITS_MANCALIBRATE]->setText(QString("Calibrate manually... [") + QLocale().toString(pxCalibrated) + QString(" px = ") + QLocale().toString(mmCalibrated) + QString(" mm]"));
		
		displayImage();
	}
	else
	{
		for(int i = 0; i < UNITS_CALIBRATE; i++) // should always find one, if not found, do nothing at all
		{
			if(unitsActions[i] == action)
			{
				//ui->unitsButton->setText("Units: " + action->text() + " ");
				unitsAction = (UnitsActionID) i;
				displaySelected(ui->chooseDisplayBox->currentIndex());
				break;
			}
		}
	}
	
}

void ProjectTab::solve()
{
	ui->openImagesButton->setEnabled(false);
	ui->aoiButton->setEnabled(false);
	ui->solverButton->setEnabled(false);
	ui->solveButton->setEnabled(false);
	ui->unitsButton->setEnabled(false);
	
	fftccSolver = std::make_unique<opencorr::FFTCC2D>(subsetRadiusX, subsetRadiusY, omp_get_num_procs());
	
	if(solverAction == SOLVER_FFTCC_NR1)
	{
		dicSolver = std::make_unique<opencorr::NR2D1>(subsetRadiusX, subsetRadiusY, maxDeformationNorm, maxIter, omp_get_num_procs());
	}
	else if(solverAction == SOLVER_FFTCC_ICGN1)
	{
		dicSolver = std::make_unique<opencorr::ICGN2D1>(subsetRadiusX, subsetRadiusY, maxDeformationNorm, maxIter, omp_get_num_procs());
	}
	else if(solverAction == SOLVER_FFTCC_ICGN2)
	{
		dicSolver = std::make_unique<opencorr::ICGN2D2>(subsetRadiusX, subsetRadiusY, maxDeformationNorm, maxIter, omp_get_num_procs());
	}
	
	strainSolver = std::make_unique<opencorr::Strain>(strainRadius, minStrainNeighbors, omp_get_num_procs());
	
    opencorr::Image2D* refImage;

    std::vector<opencorr::POI2D> firstPOI;
    
    std::vector<opencorr::POI2D>& prevPOI = firstPOI;
	
	bool notAchieved = false;
	
	for(int i = 0; i < ui->listWidget->count(); i++)
	{
		ImageListItemWidget* item = (ImageListItemWidget*) ui->listWidget->item(i);
		ImageListItemData* data = item->data();
		
		if(i == 0)
		{
			refImage = data->image.get();
			for(int y = 0; y < refImage->height; y++)
			{
				for(int x = 0; x < refImage->width; x++)
				{
					// Assumption: image got created at 0, 0
					if(x % uniformPOIDistance == uniformPOIDistance/2 && y % uniformPOIDistance == uniformPOIDistance/2 && sceneViewPolygon.containsPoint(QPointF(x, y), Qt::OddEvenFill))
					{
						firstPOI.push_back(opencorr::POI2D(x, y));
					}
				}
			}
			continue;
		}
		
		ui->statusLabel->setText("Processing " + item->text() + "...");
		qApp->processEvents(); // maintain responsiveness
        
        data->poi = prevPOI; // copy

        fftccSolver->setImages(*refImage, *(data->image.get()));
        fftccSolver->prepare();
        fftccSolver->compute(data->poi);

        dicSolver->setImages(*refImage, *(data->image.get()));
        dicSolver->prepare();
        dicSolver->compute(data->poi);
        
        for(const opencorr::POI2D& i : data->poi)
        {
            if(i.result.convergence >= maxDeformationNorm && !notAchieved)
            {
                notAchieved = true;
            }
        }
        
        // TODO this is terrible, but calculating strain live is less accurate for some reason...
        strainSolver->setApproximation(2);
        strainSolver->prepare(data->poi);
        strainSolver->compute(data->poi);
        
        data->greenPOI = data->poi;
        
        strainSolver->setApproximation(1);
        strainSolver->prepare(data->poi);
        strainSolver->compute(data->poi);
		
		prevPOI = data->poi; // reference
    }
    
    if(notAchieved)
    {
        QMessageBox::warning(nullptr, "Warning", QString("Convergence of ") + QLocale().toString(maxDeformationNorm) + QString(" not achieved at some points after ") + QString::number(maxIter) + QString(" iterations"));
    }
	
	ui->statusLabel->setText("Ready");
	displayImage();
	ui->openImagesButton->setEnabled(true);
	ui->aoiButton->setEnabled(true);
	ui->solverButton->setEnabled(true);
	ui->solveButton->setEnabled(true);
	ui->unitsButton->setEnabled(true);
}

static void drawPoint(QGraphicsScene* scene, double x, double y, QColor color)
{
	QPen pen = QPen(QBrush(color), 8.0);
	pen.setCosmetic(true);
	
	QGraphicsLineItem* circle = new QGraphicsLineItem(x-0.0001, y, x+0.0001, y); // have to move the point at least a little for it to appear
	circle->setPen(pen);
	scene->addItem(circle);
}

static void drawArrow(QGraphicsScene* scene, double fromX, double fromY, double toX, double toY, QColor color)
{
	if(toX == fromX && toY == fromY)
	{
		drawPoint(scene, fromX, fromY, color);
		return;
	}
	
	QBrush brush = QBrush(color);
	QPen pen = QPen(brush, 4.0, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
	pen.setCosmetic(true);
	
	QLineF vector = QLineF(toX, toY, fromX, fromY); // inverted...
	
	QPolygonF arrowhead;
	arrowhead << QPointF(toX, toY);
	
	QLineF wing = vector.unitVector(); // ...so that this starts at the end
	
	QGraphicsLineItem* line = new QGraphicsLineItem(fromX, fromY, wing.x2(), wing.y2());
	line->setPen(pen);
	scene->addItem(line);
	
	wing.setLength(1.154700538); // 1/cos(30deg)
	wing.setAngle(wing.angle() - 30.0);
	arrowhead << QPointF(wing.x2(), wing.y2());
	wing.setAngle(wing.angle() + 60.0);
	arrowhead << QPointF(wing.x2(), wing.y2());
	
	arrowhead << QPointF(toX, toY);
	
	QGraphicsPolygonItem* polygon = new QGraphicsPolygonItem(arrowhead);
	polygon->setBrush(brush);
	polygon->setPen(pen);
	scene->addItem(polygon);
	
	// drawCircle(scene, toX-0.75, toY-0.75, 1.5, color);
}

void ProjectTab::displayImage()
{
	scene->clear();
	
	ImageListItemWidget* item = (ImageListItemWidget*) ui->listWidget->currentItem();
	if(!item)
	{
		scene->setSceneRect(0, 0, 0, 0);
		ui->sceneView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
		return;
	}
	
	ImageListItemData* data = item->data(); 
	
	QGraphicsPixmapItem* pi = new QGraphicsPixmapItem(data->pixmap);
	scene->addItem(pi);
	
	float minValue = FLT_MAX, maxValue = -FLT_MAX;
	
	for(opencorr::POI2D& i : data->poi) //only for minima and maxima
	{
		if(displayType.unit == UNIT_TYPE_DEFORMATION)
		{
			float displacementX = displayType.subUnit != 1 ? i.deformation.u : 0.0f;
			float displacementY = displayType.subUnit != 0 ? i.deformation.v : 0.0f;
			float displacement = sqrtf(powf(displacementX, 2) + powf(displacementY, 2));
			
			if(displacement < minValue) minValue = displacement;
			if(displacement > maxValue) maxValue = displacement;
		}
		else if(displayType.unit == UNIT_TYPE_CAUCHY_STRAIN)
		{
			float strain = (displayType.subUnit == 0 ? i.strain.exx : displayType.subUnit == 1 ? i.strain.eyy : i.strain.exy) * 100.0;
			
			if(strain < minValue) minValue = strain;
			if(strain > maxValue) maxValue = strain;
		}
		else if(displayType.unit == UNIT_TYPE_MAX)
		{
			if(displayType.subUnit == 1 || displayType.subUnit == 2)
			{
				float convergence = i.result.convergence;
				
				if(convergence < minValue) minValue = convergence;
				if(convergence > maxValue) maxValue = convergence;
			}
			else break;
		}
		else break;
	}
	
	// TODO this is terrible, but calculating strain live is less accurate for some reason...
	for(opencorr::POI2D& i : data->greenPOI)
	{
		if(displayType.unit == UNIT_TYPE_GREEN_STRAIN)
		{
			
			float strain = (displayType.subUnit == 0 ? i.strain.exx : displayType.subUnit == 1 ? i.strain.eyy : i.strain.exy) * 100.0;
			
			if(strain < minValue) minValue = strain;
			if(strain > maxValue) maxValue = strain;
		}
		else break;
	}
	
	if(minValue != FLT_MAX)
	{
		double factor = (displayType.unit == UNIT_TYPE_DEFORMATION && unitsAction == UNITS_MM_PERCENT) ? mmPerPxFactor : 1.0;
		if(customMin)
		{
			minValue = ui->minSpinBox->value() / factor;
		}
		else
		{
			ui->minSpinBox->blockSignals(true);
			ui->minSpinBox->setValue(minValue * factor);
			ui->minSpinBox->blockSignals(false);
		}
		if(customMax)
		{
			maxValue = ui->maxSpinBox->value() / factor;
		}
		else
		{
			ui->maxSpinBox->blockSignals(true);
			ui->maxSpinBox->setValue(maxValue * factor);
			ui->maxSpinBox->blockSignals(false);
		}
		ui->colormap->setMinValue(ui->minSpinBox->value());
		ui->colormap->setMaxValue(ui->maxSpinBox->value());
	}
	else
	{
		ui->colormap->setMinValue(0.0);
		ui->colormap->setMaxValue(0.0);
	}
	ui->colormap->update();
	
	
	for(opencorr::POI2D& i : data->poi)
	{
		if(i.result.zncc <= 0) continue;
		if(displayType.unit == UNIT_TYPE_DEFORMATION)
		{
			float displacementX = displayType.subUnit != 1 ? i.deformation.u : 0.0f;
			float displacementY = displayType.subUnit != 0 ? i.deformation.v : 0.0f;
			float displacement = sqrtf(powf(displacementX, 2) + powf(displacementY, 2));
			
			QColor color;
			
			if((displacement - minValue) / (maxValue - minValue) > 1.0)
			{
				color = ui->aboveMaxColorButton->getColor();
			}
			else if((displacement - minValue) / (maxValue - minValue) < 0.0)
			{
				color = ui->belowMinColorButton->getColor();
			}
			else
			{
				color = ui->colormap->getColor((displacement - minValue) / (maxValue - minValue));
			}
			
			drawArrow(scene, i.x, i.y, i.x + displacementX, i.y + displacementY, color);
		}
		else if(displayType.unit == UNIT_TYPE_CAUCHY_STRAIN)
		{
            float displacementX = i.deformation.u;
            float displacementY = i.deformation.v;
			float strain = (displayType.subUnit == 0 ? i.strain.exx : displayType.subUnit == 1 ? i.strain.eyy : i.strain.exy) * 100.0;
			
			QColor color;
			
			if((strain - minValue) / (maxValue - minValue) > 1.0)
			{
				color = ui->aboveMaxColorButton->getColor();
			}
			else if((strain - minValue) / (maxValue - minValue) < 0.0)
			{
				color = ui->belowMinColorButton->getColor();
			}
			else
			{
				color = ui->colormap->getColor((strain - minValue) / (maxValue - minValue));
			}
			
            drawPoint(scene, i.x + displacementX, i.y + displacementY, color);
		}
		else if(displayType.unit == UNIT_TYPE_MAX)
		{
			float displacementX;
            float displacementY;
            if(displayType.subUnit == 1)
            {
                displacementX = 0.0f;
                displacementY = 0.0f;
            }
            else if(displayType.subUnit == 2)
            {
                displacementX = i.deformation.u;
                displacementY = i.deformation.v;
            }
            else break;
            
			float convergence = i.result.convergence;
			
			QColor color;
			
			if((convergence - minValue) / (maxValue - minValue) > 1.0)
			{
				color = ui->aboveMaxColorButton->getColor();
			}
			else if((convergence - minValue) / (maxValue - minValue) < 0.0)
			{
				color = ui->belowMinColorButton->getColor();
			}
			else
			{
				color = ui->colormap->getColor((convergence - minValue) / (maxValue - minValue));
			}
			
            drawPoint(scene, i.x + displacementX, i.y + displacementY, color);
		}
		else break;
	}
	
	// TODO this is terrible, but calculating strain live is less accurate for some reason...
	for(opencorr::POI2D& i : data->greenPOI)
	{
		if(i.result.zncc <= 0) continue;
		if(displayType.unit == UNIT_TYPE_GREEN_STRAIN)
		{
			float displacementX = i.deformation.u;
			float displacementY = i.deformation.v;
			float strain = (displayType.subUnit == 0 ? i.strain.exx : displayType.subUnit == 1 ? i.strain.eyy : i.strain.exy) * 100.0;
			
			QColor color;
			
			if((strain - minValue) / (maxValue - minValue) > 1.0)
			{
				color = ui->aboveMaxColorButton->getColor();
			}
			else if((strain - minValue) / (maxValue - minValue) < 0.0)
			{
				color = ui->belowMinColorButton->getColor();
			}
			else
			{
				color = ui->colormap->getColor((strain - minValue) / (maxValue - minValue));
			}
			
			drawPoint(scene, i.x + displacementX, i.y + displacementY, color);
		} else break;
	}
}

bool ProjectTab::eventFilter(QObject* object, QEvent* event)
{
	if(object == scene)
	{
		if(event->type() == QEvent::GraphicsSceneMousePress && sceneViewMode == MODE_TWOPOINTS_POINT1)
		{
			sceneViewPoint1 = ((QGraphicsSceneMouseEvent*) event)->scenePos();
			ui->statusLabel->setText("Click to grab coordinate 2, or Esc to cancel");
			sceneViewMode = MODE_TWOPOINTS_POINT2;
			return true;
		}
		else if(sceneViewMode == MODE_TWOPOINTS_POINT2)
		{
			if(event->type() == QEvent::GraphicsSceneMouseMove)
			{
				QPointF pos = ((QGraphicsSceneMouseEvent*) event)->scenePos();
				ui->sceneView->ruler = QLineF(sceneViewPoint1.x(), sceneViewPoint1.y(), pos.x(), pos.y());
				scene->update();
			}
			else if(event->type() == QEvent::GraphicsSceneMousePress)
			{
				sceneViewPoint2 = ((QGraphicsSceneMouseEvent*) event)->scenePos();
				double x1 = sceneViewPoint1.x();
				double x2 = sceneViewPoint2.x();
				double y1 = sceneViewPoint1.y();
				double y2 = sceneViewPoint2.y();
				double px = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
				bool got;
				double mm = QInputDialog::getDouble(this, "Calibrate units...", QString("Selected ") + QLocale().toString(px) + QString(" px. Enter dimension in millimeters:"), mmCalibrated, DBL_MIN, DBL_MAX, 15, &got);
				if(got)
				{
					pxCalibrated = px;
					mmCalibrated = mm;
					mmPerPxFactor = mmCalibrated/pxCalibrated;
					unitsActions[UNITS_MANCALIBRATE]->setText(QString("Calibrate manually... [") + QLocale().toString(pxCalibrated) + QString(" px = ") + QLocale().toString(mmCalibrated) + QString(" mm]"));
					displayImage();
				}
				ui->statusLabel->setText("Ready");
				ui->sceneView->setCursor(Qt::ArrowCursor);
				sceneViewMode = MODE_NORMAL;
				ui->openImagesButton->setEnabled(true);
				ui->aoiButton->setEnabled(true);
				ui->solverButton->setEnabled(true);
				ui->solveButton->setEnabled(true);
				ui->unitsButton->setEnabled(true);
				ui->sceneView->ruler = QLineF();
				scene->update();
				return true;
			}
		}
		else if(sceneViewMode == MODE_POLYGON)
		{
			if(event->type() == QEvent::GraphicsSceneMouseMove)
			{
				if(ui->sceneView->points.count() > 1)
				{
					QPointF pos = ((QGraphicsSceneMouseEvent*) event)->scenePos();
					ui->sceneView->points[ui->sceneView->points.count()-1] = pos;
					scene->update();
				}
			}
			else if(event->type() == QEvent::GraphicsSceneMousePress && ((QGraphicsSceneMouseEvent*) event)->button() == Qt::RightButton)
			{
				if(ui->sceneView->points.count() > 1)
				{
					ui->sceneView->points.pop_back();
					// Remove both points if just one remains
					if(ui->sceneView->points.count() == 1)
					{
						ui->sceneView->points.pop_back();
					}
					else // move the last point to the mouse
					{
						QPointF pos = ((QGraphicsSceneMouseEvent*) event)->scenePos();
						ui->sceneView->points[ui->sceneView->points.count()-1] = pos;
					}
					scene->update();
				}
				return true;
			}
			else if(event->type() == QEvent::GraphicsSceneMousePress && ((QGraphicsSceneMouseEvent*) event)->button() == Qt::LeftButton)
			{
				QPointF pos = ((QGraphicsSceneMouseEvent*) event)->scenePos();
				// Append first point twice
				// The second point will move with the mouse
				// Then when it is clicked, another is appended
				// And then that one will move with the mouse
				if(ui->sceneView->points.count() == 0)
				{
					ui->sceneView->points.append(pos);
				}
				ui->sceneView->points.append(pos);
				scene->update();
				return true;
			}
		}
	}
	else if(event->type() == QEvent::KeyRelease)
	{
		int key = ((QKeyEvent*) event)->key();
		if(key == Qt::Key_Escape && sceneViewMode != MODE_NORMAL)
		{
			if(sceneViewMode == MODE_POLYGON)
			{
				ui->sceneView->points = ui->sceneView->savedPoints;
			}
			else
			{
				ui->sceneView->ruler = QLineF();
			}
			ui->statusLabel->setText("Ready");
			ui->sceneView->setCursor(Qt::ArrowCursor);
			sceneViewMode = MODE_NORMAL;
			ui->openImagesButton->setEnabled(true);
			ui->aoiButton->setEnabled(true);
			ui->solverButton->setEnabled(true);
			ui->solveButton->setEnabled(true);
			ui->unitsButton->setEnabled(true);
			scene->update();
			return true;
		}
		else if(key == Qt::Key_Return && sceneViewMode == MODE_POLYGON)
		{
			// Remove the dynamic point
			if(ui->sceneView->points.count() > 0)
			{
				ui->sceneView->points.pop_back();
			}
			// Connect the last point
			if(ui->sceneView->points.count() > 2)
			{
				ui->sceneView->points.append(ui->sceneView->points[0]);
			}
			
			ui->sceneView->savedPoints = ui->sceneView->points;
			sceneViewPolygon = QPolygonF(ui->sceneView->points);
			scene->update();
			ui->statusLabel->setText("Ready");
			ui->sceneView->setCursor(Qt::ArrowCursor);
			sceneViewMode = MODE_NORMAL;
			ui->openImagesButton->setEnabled(true);
			ui->aoiButton->setEnabled(true);
			ui->solverButton->setEnabled(true);
			ui->solveButton->setEnabled(true);
			ui->unitsButton->setEnabled(true);
			return true;
		}
	}
	return false;
}

ProjectTab::~ProjectTab()
{
	delete ui;
	
	delete solverActionGroup;
	delete solverMenu;
	for(int i = 0; i < SOLVER_MAX; i++)
	{
		delete solverActions[i];
	}
	
	delete unitsActionGroup;
	delete unitsMenu;
	for(int i = 0; i < UNITS_MAX; i++)
	{
		delete unitsActions[i];
	}
	
	delete scene;
}

void ProjectTab::on_maxSpinBox_valueChanged(double value)
{
	customMax = true;
	displayImage();
}

void ProjectTab::on_minSpinBox_valueChanged(double value)
{
	customMin = true;
	displayImage();
}

void ProjectTab::resetMax()
{
	customMax = false;
	displayImage();
}

void ProjectTab::resetMin()
{
	customMin = false;
	displayImage();
}

void ProjectTab::on_aoiButton_released()
{
	ui->statusLabel->setText("Left click to add a polygon point, right click to remove the last point, Enter to confirm area, or Esc to cancel");
	ui->sceneView->setCursor(Qt::CrossCursor);
	sceneViewMode = MODE_POLYGON;
	ui->openImagesButton->setEnabled(false);
	ui->aoiButton->setEnabled(false);
	ui->solverButton->setEnabled(false);
	ui->solveButton->setEnabled(false);
	ui->unitsButton->setEnabled(false);
	ui->sceneView->points.clear();
	scene->update();
}
