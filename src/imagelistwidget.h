#ifndef IMAGELISTWIDGET_H
#define IMAGELISTWIDGET_H

#include <QListWidget>
#include <QStyleOptionViewItem>
#include <QDesktopWidget>
#include <QSize>

class ImageListWidget : public QListWidget
{
	using QListWidget::QListWidget; //constructors
	
	public:
	QStyleOptionViewItem viewOptions() const override
	{
		QStyleOptionViewItem option = QListWidget::viewOptions();
		option.decorationPosition = QStyleOptionViewItem::Top; //put icons above text
		//option.decorationAlignment = Qt::AlignCenter; //align icons to center
		option.decorationSize = 2*QSize(qApp->desktop()->logicalDpiX(), qApp->desktop()->logicalDpiY());
		option.displayAlignment = Qt::AlignHCenter | Qt::AlignBottom; //align text to bottom center
		return option;
	}
	
	/*QSize sizeHint() const override
	{
		return QListWidget::minimumSizeHint();
	}*/
};

#endif // IMAGELISTWIDGET_H
