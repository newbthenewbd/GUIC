#ifndef IMAGELISTITEM_H
#define IMAGELISTITEM_H

#include <QListWidgetItem>
#include <QPixmap>
#include <vector>
#include "opencorr.h"

class ImageListItem : public QListWidgetItem
{
    using QListWidgetItem::QListWidgetItem; //constructors
    
    public:
    QPixmap pixmap;
    opencorr::Image2D* image;
    std::vector<opencorr::POI2D> poi = {};
};

#endif // IMAGELISTITEM_H
