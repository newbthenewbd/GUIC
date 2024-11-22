#ifndef COLORPICKERBUTTON_H
#define COLORPICKERBUTTON_H

#include <QToolButton>
#include <QColorDialog>
#include <QPainter>

class ColorPickerButton : public QToolButton
{
	Q_OBJECT
	
	using QToolButton::QToolButton; //constructors
	
	private:
	QColor color = Qt::white;
	
	signals:
	void colorSet();
	
	public:
	void setColor(QColor color);
	QColor getColor();
	
	protected:
	void paintEvent(QPaintEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
};


#endif // COLORPICKERBUTTON_H
