#include "projecttab.h"
#include "ui_projecttab.h"
#define W_NO_PROPERTY_MACRO
#include <wobjectimpl.h>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QPixmap>
#include <QImage>
#include "imagelistitem.h"
#include "opencorr.h"
#include <opencv2/opencv.hpp>
//#include <opencv2/world.hpp>
#include "turbo_colormap.h"

QGraphicsScene* scene;
QPainter painter;

W_OBJECT_IMPL(ProjectTab)

ProjectTab::ProjectTab(QWidget *parent) :
QWidget(parent),
ui(new Ui::ProjectTab)
{
	ui->setupUi(this);
	
	connect(ui->addImagesButton, &QToolButton::released, this, &ProjectTab::addImages);
	connect(ui->listWidget, &QListWidget::itemClicked, this, &ProjectTab::imageListItemSelected);
	
	scene = new QGraphicsScene();
	ui->graphicsView->setScene(scene);
}

void ProjectTab::addImages()
{
	QStringList paths = QFileDialog::getOpenFileNames(this, QObject::tr("Add images"), /*QDir::currentPath()*/ "/Users/Czcibor/Desktop/Studia/Praca/muDIC/Examples/example_data", QObject::tr("Images (*.bmp *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.tif *.tiff *.xbm *.xpm);;BMP (*.bmp);;JPEG (*.jpg *.jpeg);;PNG (*.png);;PBM (*.pbm);;PGM (*.pgm);;PPM (*.ppm);;TIFF (*.tif *.tiff);;XBM (*.xbm);;XPM (*.xpm);;All files (*.*)"));
	addImagesFromPaths(paths);
}

void ProjectTab::addImagesFromPaths(QStringList paths)
{
	int subsetRadiusX = 16;
	int subsetRadiusY = 16;
	
	opencorr::Image2D* refImage;
	
	opencorr::FFTCC2D* fftcc = new opencorr::FFTCC2D(subsetRadiusX, subsetRadiusY, omp_get_num_procs());
	std::vector<opencorr::POI2D> poi;
	int maxIter = 10;
	float maxDeformationNorm = 0.001f;
	opencorr::NR2D1* nr = new opencorr::NR2D1(subsetRadiusX, subsetRadiusY, maxDeformationNorm, maxIter, omp_get_num_procs());
	
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
					
					unsigned char* color = turbo_srgb_bytes[(unsigned char) ((displacement - minDisplacement) * (255 / (maxDisplacement - minDisplacement)))];
					
					painter.begin(&item->pixmap);
					painter.setRenderHint(QPainter::Antialiasing, true);
					painter.setPen(QColor(color[0], color[1], color[2])); //outline
					//painter.setBrush(QColor(color[0], color[1], color[2])); //fill
					
					//draw arrow
					painter.drawLine(i.x, i.y, i.x - i.deformation.u, i.y - i.deformation.v);
					//int angle = atan2(i.x * i.deformation.v - i.y * i.deformation.u, i.x * i.deformation.u + i.y * i.deformation.v) * (180.0 / M_PI) * 16.0;
					//painter.drawChord(i.x-1, i.y-1, 3, 3, angle-60, 2880+120);
					painter.drawChord(QRectF(i.x-1.5, i.y-1.5, 3.0, 3.0), 0, 5760);
					
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
					
					unsigned char* color = turbo_srgb_bytes[(unsigned char) ((strain - minStrain) * (255 / (maxStrain - minStrain)))];
					
					painter.begin(&item->pixmap);
					painter.setRenderHint(QPainter::Antialiasing, true);
					painter.setPen(QColor(color[0], color[1], color[2]));
					painter.setBrush(QColor(color[0], color[1], color[2])); //fill
					painter.drawChord(QRectF(i.x, i.y, 3.0, 3.0), 0, 5760);
					painter.end();
				}
			}
#endif
		}
	}
#ifdef STRAIN
	delete strain;
#endif
	//delete nr;
	delete fftcc;
}

void ProjectTab::imageListItemSelected(QListWidgetItem* imageListItem) {
	ImageListItem* item = (ImageListItem*) imageListItem;
	scene->clear();
	QGraphicsPixmapItem* pi = new QGraphicsPixmapItem(item->pixmap);
	//pi->setPos(10, 30);
	scene->addItem(pi);
}

ProjectTab::~ProjectTab()
{
	delete ui;
}
