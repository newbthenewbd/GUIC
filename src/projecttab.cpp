#include "projecttab.h"
#include "ui_projecttab.h"
#define W_NO_PROPERTY_MACRO
#include <wobjectimpl.h>
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
#include <QListWidgetItem>
#include <QStyleFactory>
#include <QMenu>
#include <QAction>
#include <QComboBox>
#include <QInputDialog>
#include "imagelistitem.h"
#include "opencorr.h"
#include <omp.h>
#include <opencv2/opencv.hpp>
//#include <opencv2/world.hpp>
#define TINYCOLORMAP_WITH_QT5
#include "tinycolormap.hpp"
#include <vector>

W_OBJECT_IMPL(ProjectTab)

ProjectTab::ProjectTab(QWidget* parent) :
QWidget(parent),
ui(new Ui::ProjectTab)
{
	ui->setupUi(this);
	
	scene = new QGraphicsScene();
	ui->sceneView->setScene(scene);
	
	colormapSelected(0);
	
	QStyle* fusion = QStyleFactory::create("Fusion"); //owned by Qt
	
	ui->displayConfigButton->setStyle(fusion);
	ui->displayConfigButton->setIcon(style()->standardIcon(QStyle::SP_FileDialogNewFolder));
	
	for(QObject* object : ui->toolBar->children())
	{
		if(object->isWidgetType()) ((QWidget*) object)->setStyle(fusion);
	}
	
	ui->saveProjectButton->setIcon(style()->standardIcon(QStyle::SP_DriveFDIcon));
	
	ui->openImagesButton->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
	
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
		//Load image into Qt
		QFileInfo fileInfo(*i);
		ImageListItem* item = new ImageListItem();
		item->pixmap = QPixmap(*i);
		ui->statusLabel->setText("Loading " + fileInfo.fileName() + "...");
		item->setText(fileInfo.fileName()); //TODO check if the name is unique
		
		item->image = new opencorr::Image2D(item->pixmap.width(), item->pixmap.height());
		QImage inImage = item->pixmap.toImage().convertToFormat(QImage::Format_Grayscale8); //to be sure
		item->image->cv_mat = cv::Mat(inImage.height(), inImage.width(), CV_8UC1, inImage.bits(), (size_t) inImage.bytesPerLine()); //Taken from [1]; thanks!
		cv::cv2eigen(item->image->cv_mat, item->image->eg_mat);
		
		item->pixmap = QPixmap::fromImage(inImage);
		item->setIcon(item->pixmap);
		ui->listWidget->addItem(item);
		qApp->processEvents(); //maintain responsiveness
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
		ImageListItem* item = (ImageListItem*) ui->listWidget->item(i);
		
		if(i == 0)
		{
			refImage = item->image;
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
			
			if(item->poi.empty())
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
			} else poi = item->poi;
		}
		
		fftcc->setImages(*refImage, *item->image);
		fftcc->prepare();
		fftcc->compute(poi);
		
		nr->setImages(*refImage, *item->image);
		nr->prepare();
		nr->compute(poi);
		
		strain->setApproximation(1); //Cauchy strain
		strain->prepare(poi);
		strain->compute(poi);
		
		item->poi = poi;
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
	ImageListItem* item = (ImageListItem*) ui->listWidget->currentItem();
	if(item == NULL) return;
	
	scene->clear();
	QGraphicsPixmapItem* pi = new QGraphicsPixmapItem(item->pixmap);
	scene->addItem(pi);
	
	float minDisplacement = FLT_MAX, maxDisplacement = FLT_MIN;
	float minStrain = FLT_MAX, maxStrain = FLT_MIN;
	
	for(opencorr::POI2D& i : item->poi) //only for minima and maxima
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
	
	
	for(opencorr::POI2D& i : item->poi)
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
			
			drawCircle(scene, i.x, i.y, 8.0, color);
		}
	}
}

#if 0
	for(QStringList::iterator i = paths.begin(); i != paths.end(); i++)
	{
		//Load image into Qt
		QFileInfo fileInfo(*i);
		ImageListItem* item = new ImageListItem();
		item->pixmap = QPixmap(*i);
		item->setText(fileInfo.fileName()); //TODO check if the name is unique
		item->setIcon(item->pixmap);
		ui->listWidget->addItem(item);
		
		//Load image into OpenCorr
		item->image = new opencorr::Image2D(item->pixmap.width(), item->pixmap.height());
		QImage inImage = item->pixmap.toImage().convertToFormat(QImage::Format_Grayscale8);
		item->image->cv_mat = cv::Mat(inImage.height(), inImage.width(), CV_8UC1, inImage.bits(), (size_t) inImage.bytesPerLine()); //Taken from [1]; thanks!
		cv::cv2eigen(item->image->cv_mat, item->image->eg_mat);
		
		if(ui->listWidget->count() <= 1) //0 should never happen; set reference image
		{
			printf("Setting reference image to %s...\n", qPrintable(fileInfo.fileName()));
			
			refImage = item->image;
		}
		else
		{
			printf("Processing %s...\n", qPrintable(fileInfo.fileName()));
			
			if(ui->listWidget->count() == 2) //No previous item to start from
			{
				//Set points of interest
				for(int y = 800; y < 1600; y++)
				{
					for(int x = 325; x < 525; x++)
					{
						if(x % 8 == 0 && y % 8 == 0)
						{
							item->poi.push_back(opencorr::POI2D(x, y));
						}
					}
				}
				
				fftcc->setImages(*refImage, *item->image);
				fftcc->prepare();
				fftcc->compute(item->poi);
			}
			else
			{
				item->poi = ((ImageListItem*) ui->listWidget->item(ui->listWidget->count()-2))->poi;
			}
			
			nr->setImages(*refImage, *item->image);
			nr->prepare();
			nr->compute(item->poi);
			
#ifndef STRAIN
			for(opencorr::POI2D& i : item->poi)
			{
				float displacement = sqrtf(powf(i.deformation.u, 2) + powf(i.deformation.v, 2));
				
				if(displacement < minDisplacement) minDisplacement = displacement;
				if(displacement > maxDisplacement) maxDisplacement = displacement;
			}
			printf("min %f max %f\n", minDisplacement, maxDisplacement);
			for(opencorr::POI2D& i : item->poi)
			{
				float displacement = sqrtf(powf(i.deformation.u, 2) + powf(i.deformation.v, 2));
				
				if(displacement != 0.0f)
				{
					//printf("Displacement at %f %f: %f\n", i.x, i.y, displacement);
					
					//unsigned char* color = turbo_srgb_bytes[(unsigned char) ((displacement - minDisplacement) * (255 / (maxDisplacement - minDisplacement)))];
					QColor color = ui->colormap->getColor((displacement - minDisplacement) / (maxDisplacement - minDisplacement));
					
					painter.begin(&item->pixmap);
					painter.setRenderHint(QPainter::Antialiasing, true);
					painter.setPen(color); //outline
					//painter.setBrush(color); //fill
					
					//draw arrow
					painter.drawLine(i.x, i.y, i.x + i.deformation.u, i.y + i.deformation.v);
					//int angle = atan2(i.x * i.deformation.v - i.y * i.deformation.u, i.x * i.deformation.u + i.y * i.deformation.v) * (180.0 / M_PI) * 16.0;
					//painter.drawChord(i.x-1, i.y-1, 3, 3, angle-60, 2880+120);
					painter.drawChord(QRectF(i.x + i.deformation.u - 1.5f, i.y + i.deformation.v - 1.5f, 3.0f, 3.0f), 0, 5760);
					//painter.drawChord(QRectF(i.x - 1.5f, i.y - 1.5f, 3.0f, 3.0f), 0, 5760);
					
					painter.end();
				}
			}
#else
			//Calculate strains
			strain->setApproximation(2); //Green strain
			strain->prepare(item->poi);
			strain->compute(item->poi);
			
			for(opencorr::POI2D& i : item->poi)
			{
				//float strain = cbrtf(powf(i.strain.exx, 3) + powf(i.strain.eyy, 3) + powf(i.strain.exy, 3));
				float strain = i.strain.exx;
				
				if(strain < minStrain) minStrain = strain;
				if(strain > maxStrain) maxStrain = strain;
			}
			printf("min %f max %f\n", minStrain, maxStrain);
			for(opencorr::POI2D& i : item->poi)
			{
				//float strain = cbrtf(powf(i.strain.exx, 3) + powf(i.strain.eyy, 3) + powf(i.strain.exy, 3));
				float strain = i.strain.exx;
				
				if(strain != 0.0f)
				{
					//printf("Strain at %f %f: %f\n", i.x, i.y, strain);
					
					//unsigned char* color = turbo_srgb_bytes[(unsigned char) ((strain - minStrain) * (255 / (maxStrain - minStrain)))];
					QColor color = ui->colormap->getColor((strain - minStrain) / (maxStrain - minStrain));
					
					painter.begin(&item->pixmap);
					painter.setRenderHint(QPainter::Antialiasing, true);
					painter.setPen(color);
					painter.setBrush(color); //fill
					painter.drawChord(QRectF(i.x, i.y, 8.0, 8.0), 0, 5760);
					painter.end();
				}
			}
#endif
		}
	}
	
#if 0
//#define STRAIN
	
#ifdef STRAIN
	float strainRadius = 20.f;
	int minNeighbors = 5;
	opencorr::Strain* strain = new opencorr::Strain(strainRadius, minNeighbors, omp_get_num_procs());
#endif
	float minDisplacement = FLT_MAX, maxDisplacement = FLT_MIN;
	float minStrain = FLT_MAX, maxStrain = FLT_MIN;
	for(QStringList::iterator i = paths.begin(); i != paths.end(); i++)
	{
		//Load image into Qt
		QFileInfo fileInfo(*i);
		ImageListItem* item = new ImageListItem();
		item->pixmap = QPixmap(*i);
		item->setText(fileInfo.fileName()); //TODO check if the name is unique
		item->setIcon(item->pixmap);
		ui->listWidget->addItem(item);
		
		//Load image into OpenCorr
		item->image = new opencorr::Image2D(item->pixmap.width(), item->pixmap.height());
		QImage inImage = item->pixmap.toImage().convertToFormat(QImage::Format_Grayscale8);
		item->image->cv_mat = cv::Mat(inImage.height(), inImage.width(), CV_8UC1, inImage.bits(), (size_t) inImage.bytesPerLine()); //Taken from [1]; thanks!
		cv::cv2eigen(item->image->cv_mat, item->image->eg_mat);
		
		//Set points of interest
		for(int y = 1000; y < 1300; y++)
		{
			for(int x = 300; x < 550; x++)
			{
				if(x % 8 == 0 && y % 8 == 0)
				{
					item->poi.push_back(opencorr::POI2D(opencorr::Point2D(x, y)));
				}
			}
		}
		
		if(i == paths.begin())
		{
			printf("Setting reference image to %s...\n", qPrintable(fileInfo.fileName()));
			refImage = item->image;
		}
		else //TODO make sure the following images are the same size
		{
			fftcc->setImages(*refImage, *item->image);
			nr->setImages(*refImage, *item->image);
			
			printf("Processing %s...\n", qPrintable(fileInfo.fileName()));
			
			//Calculate displacements
			fftcc->prepare();
			fftcc->compute(item->poi);
			
			//nr->prepare();
			//nr->compute(item->poi);

		}
	}
#ifdef STRAIN
	delete strain;
#endif
#endif
	//delete nr;
	delete fftcc;
#endif

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
