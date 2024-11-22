#ifndef IMAGELISTWIDGET_H
#define IMAGELISTWIDGET_H

#include <QListWidget>
#include <QStyleOptionViewItem>
#include <QDesktopWidget>
#include <QSize>
#include <QMenu>
#include <QContextMenuEvent>
#include "imagelistitemwidget.h"

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
	
	protected:
	void contextMenuEvent(QContextMenuEvent* event) override
	{
		ImageListItemWidget* item = (ImageListItemWidget*) itemAt(event->pos());
		if(!item) return;
		
		QMenu contextMenu(this);
		
		QAction* renameAction = new QAction("Rename", this);
		QAction* deleteAction = new QAction("Delete", this);
		
		connect(renameAction, &QAction::triggered, [this, item]()
		{
			editItem(item);
		});
		connect(deleteAction, &QAction::triggered, [this]()
		{
			for(int i = this->count()-1; i >= 0; i--)
			{
				ImageListItemWidget* item = (ImageListItemWidget*) this->item(i);
				if(item->isSelected())
				{
					ImageListItemData* data = item->data();
					delete data;
					delete item;
				}
			}
		});
		
		contextMenu.addAction(renameAction);
		contextMenu.addAction(deleteAction);
		contextMenu.exec(mapToGlobal(event->pos()));
	}
	
	void keyPressEvent(QKeyEvent* event) override
	{
		if(event->key() == Qt::Key_Delete)
		{
			// ImageListItemWidget* item = (ImageListItemWidget*) currentItem();
			// if(item != nullptr)
			// {
			// 	delete item->data();
			// 	delete item;
			// }
			for(int i = this->count()-1; i >= 0; i--)
			{
				ImageListItemWidget* item = (ImageListItemWidget*) this->item(i);
				if(item->isSelected())
				{
					ImageListItemData* data = item->data();
					delete data;
					delete item;
				}
			}
			return;
		}
		
		QListWidget::keyPressEvent(event);
	}
};

#endif // IMAGELISTWIDGET_H
