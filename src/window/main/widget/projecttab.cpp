#include "projecttab.h"
#include "ui_projecttab.h"
#include <QObject>
#include <QEvent>
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
#include <opencorr.h>
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
	
    ui->openImagesButton->setIcon(style()->standardIcon(QStyle::SP_FileDialogStart));
	
	ui->solverButton->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
	solverMenu = new QMenu();
	solverActions[SOLVER_FFTCC_NR] = new QAction("FFTCC + NR", nullptr);
	for(int i = 0; i < SOLVER_MAX; i++)
	{
		solverMenu->addAction(solverActions[i]);
	}
	ui->solverButton->setMenu(solverMenu);
	
	ui->unitsButton->setIcon(style()->standardIcon(QStyle::SP_FileDialogListView));
	unitsMenu = new QMenu();
	unitsActions[UNITS_PX_PERCENT] = new QAction("px, %", nullptr);
	unitsActions[UNITS_MM_PERCENT] = new QAction("mm, %", nullptr);
	unitsActions[UNITS_CALIBRATE] = new QAction("Calibrate...", nullptr);
	for(int i = 0; i < UNITS_MAX; i++)
	{
		if(i == UNITS_CALIBRATE)
		{
			unitsMenu->addSeparator();
		}
		unitsMenu->addAction(unitsActions[i]);
	}
	ui->unitsButton->setMenu(unitsMenu);
	
	ui->solveButton->setIcon(style()->standardIcon(QStyle::SP_DialogOkButton));
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
	for(QStringList::iterator i = paths.begin(); i != paths.end(); i++)
	{
		QString path = *i;
		QFileInfo fileInfo(*i);
		ui->statusLabel->setText("Loading " + fileInfo.fileName() + "...");
		
		try {
			ImageListItemWidget* item = new ImageListItemWidget();
			item->setText(fileInfo.fileName()); //TODO check if the name is unique
			
			ImageListItemData* data = new ImageListItemData();
			item->setData(data);
			
			data->image = new opencorr::Image2D(path.toStdString());
			
			data->pixmap = QPixmap::fromImage(QImage(data->image->cv_mat.data, data->image->width, data->image->height, data->image->width, QImage::Format_Grayscale8));
			
			item->setIcon(data->pixmap);
			ui->listWidget->addItem(item);
			qApp->processEvents(); //maintain responsiveness
		} catch(...) {
			QMessageBox::critical(nullptr, "Error", "Could not load " + fileInfo.fileName());
			break;
		}
	}
	ui->statusLabel->setText("Ready");
}

//displayImage was here

void ProjectTab::displaySelected(int displayId)
{
	displayType = displayTypes[displayId];
	if(displayType.unit == UNIT_TYPE_MAX)
	{
		ui->colormap->setUnit("");
	}
	else
	{
		ui->colormap->setUnit(unitsUnits[unitsAction][displayType.unit]);
	}
	displayImage();
}

void ProjectTab::colormapSelected(int colormapId)
{
	tinycolormap::ColormapType colormapBoxTypes[] = { //TODO refactor out... along with the whole selector
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
		tinycolormap::ColormapType::Github,
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

void ProjectTab::unitsChanged(QAction* action)
{
	//QCoreApplication::postEvent(ui->unitsButton, new QHoverEvent(QEvent::HoverLeave, QPointF(), QPointF())); //no bueno, doesn't unhover the button still
	
	if(action == unitsActions[UNITS_CALIBRATE])
	{
		//TODO draw to calibrate
		/*ui->statusLabel->setText("Click and drag to grab a dimension");
		this->installEventFilter(this);*/
		
		bool got;
		
		double px = QInputDialog::getDouble(this, "Calibrate...", "Enter dimension in pixels:", pxCalibrated, 0.1, 2147483647.0, 1, &got);
		if(!got) return;
		
		double mm = QInputDialog::getDouble(this, "Calibrate...", "Enter dimension in millimeters:", mmCalibrated, 0.1, 2147483647.0, 1, &got);
		if(!got) return;
		
		pxCalibrated = px;
		mmCalibrated = mm;
		mmPerPxFactor = mm/px;
		
		displayImage();
	}
	else
	{
		for(int i = 0; i < UNITS_CALIBRATE; i++) //should always find one, if not found, do nothing at all
		{
			if(unitsActions[i] == action)
			{
				ui->unitsButton->setText("Units: " + action->text() + " ");
				unitsAction = (UnitsActionID) i;
				displaySelected(ui->chooseDisplayBox->currentIndex());
				break;
			}
		}
	}
	
}

void ProjectTab::solve()
{

	int subsetRadiusX = 16;
	int subsetRadiusY = 16;
	opencorr::FFTCC2D* fftcc;
	
	int maxIter = 10;
	float maxDeformationNorm = 0.001f;
	opencorr::NR2D1* nr;
	
	float strainRadius = 20.f;
	int minNeighbors = 5;
	opencorr::Strain* strain;
	
	opencorr::Image2D* refImage;
	
	std::vector<opencorr::POI2D> poi;
	
	for(int i = 0; i < ui->listWidget->count(); i++)
	{
		ImageListItemWidget* item = (ImageListItemWidget*) ui->listWidget->item(i);
		ImageListItemData* data = item->data();
		
		if(i == 0)
		{
			refImage = data->image;
			continue;
		}
		
		ui->statusLabel->setText("Processing " + item->text() + "...");
		qApp->processEvents(); //maintain responsiveness
		
		if(i == 1) //oh no, really have to calculate
		{
			//TODO other solvers, not just FFTCC + NR
			fftcc = new opencorr::FFTCC2D(subsetRadiusX, subsetRadiusY, omp_get_num_procs());
			nr = new opencorr::NR2D1(subsetRadiusX, subsetRadiusY, maxDeformationNorm, maxIter, omp_get_num_procs());
			strain = new opencorr::Strain(strainRadius, minNeighbors, omp_get_num_procs());
			
			if(data->poi.empty())
			{
				for(int y = 772; y < 1468; y++)
				{
					for(int x = 341; x < 511; x++)
					{
						if(x % 8 == 0 && y % 8 == 0)
						{
                            poi.push_back(opencorr::POI2D(x, y));
						}
					}
				}
            } else poi = data->poi;
		}
		
		fftcc->setImages(*refImage, *(data->image));
		fftcc->prepare();
        fftcc->compute(poi);
		
		nr->setImages(*refImage, *(data->image));
		nr->prepare();
        nr->compute(poi);
		
		strain->setApproximation(1); //Cauchy strain
        strain->prepare(poi);
        strain->compute(poi);

        data->poi = poi;
	}
	
	ui->statusLabel->setText("Ready");
	displayImage();
	
	if(ui->listWidget->count() > 1) //really calculated
	{
		delete strain;
		delete nr;
		delete fftcc;
	}
}

static void drawCircle(QGraphicsScene* scene, double x, double y, double diameter, QColor color)
{
	QGraphicsEllipseItem* ei = new QGraphicsEllipseItem(x, y, diameter, diameter);
	ei->setPen(color);
	ei->setBrush(color);
	scene->addItem(ei);
}

static void drawArrow(QGraphicsScene* scene, double fromX, double fromY, double toX, double toY, QColor color)
{
	QGraphicsLineItem* li = new QGraphicsLineItem(fromX, fromY, toX, toY);
	li->setPen(QPen(QBrush(color), 1.0));
	scene->addItem(li);
	drawCircle(scene, toX-0.75, toY-0.75, 1.5, color);
}

void ProjectTab::displayImage()
{
	ImageListItemWidget* item = (ImageListItemWidget*) ui->listWidget->currentItem();
	if(item == NULL) return;
	
	ImageListItemData* data = item->data(); 
	
	scene->clear();
	QGraphicsPixmapItem* pi = new QGraphicsPixmapItem(data->pixmap);
	scene->addItem(pi);
	
	float minDisplacement = FLT_MAX, maxDisplacement = FLT_MIN;
	float minStrain = FLT_MAX, maxStrain = FLT_MIN;
	
	for(opencorr::POI2D& i : data->poi) //only for minima and maxima
	{
		if(displayType.unit == UNIT_TYPE_DEFORMATION)
		{
			float displacementX = displayType.subUnit != 1 ? i.deformation.u : 0.0f;
			float displacementY = displayType.subUnit != 0 ? i.deformation.v : 0.0f;
			float displacement = sqrtf(powf(displacementX, 2) + powf(displacementY, 2));
			
			if(displacement < minDisplacement) minDisplacement = displacement;
			if(displacement > maxDisplacement) maxDisplacement = displacement;
		}
		else if(displayType.unit == UNIT_TYPE_STRAIN)
		{
			float strain = (displayType.subUnit == 0 ? i.strain.exx : displayType.subUnit == 1 ? i.strain.eyy : i.strain.exy) * 100.0;
			
			if(strain < minStrain) minStrain = strain;
			if(strain > maxStrain) maxStrain = strain;
		}
	}
	
	if(displayType.unit == UNIT_TYPE_DEFORMATION && minDisplacement != FLT_MAX)
	{
		ui->colormap->setMinValue(minDisplacement);
		ui->colormap->setMaxValue(maxDisplacement);
	}
	else if(displayType.unit == UNIT_TYPE_STRAIN && minStrain != FLT_MAX)
	{
		ui->colormap->setMinValue(minStrain);
		ui->colormap->setMaxValue(maxStrain);
	}
	else
	{
		ui->colormap->setMinValue(0.0);
		ui->colormap->setMaxValue(0.0);
	}
	if(displayType.unit == UNIT_TYPE_DEFORMATION && unitsAction == UNITS_MM_PERCENT)
	{
		ui->colormap->setMinValue(ui->colormap->getMinValue() * mmPerPxFactor);
		ui->colormap->setMaxValue(ui->colormap->getMaxValue() * mmPerPxFactor);
	}
	ui->colormap->update();
	
	
	for(opencorr::POI2D& i : data->poi)
	{
		if(displayType.unit == UNIT_TYPE_DEFORMATION)
		{
			float displacementX = displayType.subUnit != 1 ? i.deformation.u : 0.0f;
			float displacementY = displayType.subUnit != 0 ? i.deformation.v : 0.0f;
			float displacement = sqrtf(powf(displacementX, 2) + powf(displacementY, 2));
			
			QColor color = ui->colormap->getColor((displacement - minDisplacement) / (maxDisplacement - minDisplacement));
			
			drawArrow(scene, i.x, i.y, i.x + displacementX, i.y + displacementY, color);
		}
		else if(displayType.unit == UNIT_TYPE_STRAIN)
		{
			float strain = (displayType.subUnit == 0 ? i.strain.exx : displayType.subUnit == 1 ? i.strain.eyy : i.strain.exy) * 100.0;
			
			QColor color = ui->colormap->getColor((strain - minStrain) / (maxStrain - minStrain));
			
            drawCircle(scene, i.x + displacementX, i.y + displacementY, 8.0, color);
		}
	}
}

bool ProjectTab::eventFilter(QObject* object, QEvent* event)
{
	//TODO type of filter discrimintor, when needed...
	if(event->type() == QEvent::KeyPress || event->type() == QEvent::MouseButtonPress)
	{
		this->removeEventFilter(this);
		object->removeEventFilter(this);
		if(object != ui->sceneView)
		{
			ui->statusLabel->setText("Ready");
			return true;
		}
	}
	return false;
}

ProjectTab::~ProjectTab()
{
	delete ui;
	
	delete solverMenu;
	for(int i = 0; i < SOLVER_MAX; i++)
	{
		delete solverActions[i];
	}
	
	delete unitsMenu;
	for(int i = 0; i < UNITS_MAX; i++)
	{
		delete unitsActions[i];
	}
	
	delete scene;
}
