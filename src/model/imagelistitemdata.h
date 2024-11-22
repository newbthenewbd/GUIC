#ifndef IMAGELISTITEMDATA_H
#define IMAGELISTITEMDATA_H

#include <QPixmap>
#include <QMetaType>
#include <memory>
#include <vector>
#include <opencorr.h>

class ImageListItemData
{
	public:
	ImageListItemData();
	QPixmap pixmap;
	std::unique_ptr<opencorr::Image2D> image;
	std::vector<opencorr::POI2D> poi;
	std::vector<opencorr::POI2D> greenPOI;
};

#endif // IMAGELISTITEMDATA_H
