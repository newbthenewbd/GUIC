#ifndef IMAGELISTITEMWIDGET_H
#define IMAGELISTITEMWIDGET_H

#include <QListWidgetItem>
#include <QVariant>
#include "model/imagelistitemdata.h"

static_assert(sizeof(qulonglong) >= sizeof(void*), "sizeof(qulonglong) isn't >= sizeof(void*), cannot proceed with hack; is there now a qulonglonglong to use instead?");

class ImageListItemWidget : public QListWidgetItem
{
    using QListWidgetItem::QListWidgetItem; //constructors
    
    public:
    void setData(ImageListItemData* data)
    {
        // Yes, this is about the easiest way to just remember a pointer...
        // No, the data cannot be put in this subclass - Qt 5.6 wants to pass
        // even internal drags and drops as MIME, as if to external applications!
        // In fact, data in subclass was the previous, seemingly-working-with-Qt-5.15
        // solution... The best one is supposedly Qt's Model-View, TODO?
        QListWidgetItem::setData(Qt::UserRole, QVariant((qulonglong) data));
    }
    
    ImageListItemData* data()
    {
        return (ImageListItemData*) QListWidgetItem::data(Qt::UserRole).toULongLong();
    }
};

#endif // IMAGELISTITEMWIDGET_H
