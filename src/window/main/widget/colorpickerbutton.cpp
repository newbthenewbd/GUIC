#include "colorpickerbutton.h"

void ColorPickerButton::setColor(QColor color)
{
	if(color.isValid())
	{
		this->color = color;
		repaint();
		emit colorSet();
	}
}

QColor ColorPickerButton::getColor()
{
	return color;
}

void ColorPickerButton::paintEvent(QPaintEvent* event)
{
	QToolButton::paintEvent(event);
	
	QPainter painter(this);
	
	painter.fillRect(width()*0.25 + 0.5 - 1.0, height()*0.25 + 0.5 - 1.0, width()*0.5 + 0.5 + 2.0, height()*0.5 + 0.5 + 2.0, Qt::lightGray);
	painter.fillRect(width()*0.25 + 0.5, height()*0.25 + 0.5, width()*0.5 + 0.5, height()*0.5 + 0.5, color);
}

void ColorPickerButton::mouseReleaseEvent(QMouseEvent* event)
{
	QToolButton::mouseReleaseEvent(event);
	setColor(QColorDialog::getColor());
}