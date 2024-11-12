#ifndef IMAGELISTITEMDATA_H
#define IMAGELISTITEMDATA_H

#include <QPixmap>
#include <QMetaType>
#include <vector>
#include <opencorr.h>

class ImageListItemData
{
	public:
	ImageListItemData();
	QPixmap pixmap;
	opencorr::Image2D* image;
	std::vector<opencorr::POI2D> poi = {};
};

Q_DECLARE_METATYPE(ImageListItemData*)

#endif // IMAGELISTITEMDATA_H
