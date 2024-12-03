#include "colormapwidget.h"
#include <QPainter>
#include <QLinearGradient>
#include <QGradientStops>
#include <QColor>


QColor ColorMapWidget::getColor(double value)
{
	return tinycolormap::GetColor(value, colormapType).ConvertToQColor();
}

void ColorMapWidget::setColormap(tinycolormap::ColormapType type)
{
	colormapType = type;
	
	QGradientStops stops;
	
	for(int i = 0; i < 255; i++)
	{
		stops << QGradientStop(i / 255.0, getColor(1.0 - i / 255.0));
	}
	
	gradient.setStops(stops);
	
	update();
}

int ColorMapWidget::getDecimals()
{
	return decimals;
}

void ColorMapWidget::setDecimals(int decimals)
{
	this->decimals = decimals;
	update();
}

double ColorMapWidget::getMinValue()
{
	return minValue;
}

void ColorMapWidget::setMinValue(double value)
{
	minValue = value;
}

double ColorMapWidget::getMaxValue()
{
	return maxValue;
}

void ColorMapWidget::setMaxValue(double value)
{
	maxValue = value;
}

const QString& ColorMapWidget::getUnit()
{
	return unit;
}

void ColorMapWidget::setUnit(const QString& unit)
{
	this->unit = unit;
	update();
	emit unitChanged(unit);
}

void ColorMapWidget::paintEvent(QPaintEvent* event)
{
	(void) event;

	QPainter painter(this);
	
	painter.fillRect(0, 0, width(), height(), Qt::lightGray);
	
	int h = height()-2;
	
	gradient.setFinalStop(0, h);
	
	//painter.fillRect(0, 0, 32, height(), Qt::black);
	painter.fillRect(1, 1, 32-2, h, gradient);
	
	for(int i = 0; i < 13; i++)
	{
		painter.fillRect(31, i*h/25.0+1, 9, 1, getColor(1.0-i/25.0));
		painter.fillRect(40, i*h/26.0+1, 14, h/26.0+1.0, getColor(1.0-i/25.0));
		
		painter.fillRect(31, h-i*h/25.0, 9, 1, getColor(i/25.0));
		painter.fillRect(40, h-i*(h/26.0)+1, 14, -h/26.0-1.0, getColor(i/25.0));
	}
	
	QFont font = QFont("Courier New");
	font.setPixelSize(h/26.0 > 1 ? h/26.0 : 1);
	painter.setFont(font);
	painter.setPen(Qt::black);
	
	int minWidth = 0;
	
	for(int i = 0; i < 26; i++)
	{
		QString text = QString::number(minValue + i * (maxValue - minValue) / 25.0, 'f', decimals) + " " + unit; //TU BYŁO ŹLE! maxValue - minValue jeśli maxValue np. -0,10 minValue np. -0,15 to wychodzi mi 0,05!
		painter.drawText(56, ((25 - i) + 0.85) * h / 26.0, text);
		if(painter.fontMetrics().boundingRect(text).width() + 56 + 2 > minWidth)
		{
			minWidth = painter.fontMetrics().boundingRect(text).width() + 56 + 2;
		}
	}
	
	this->setMinimumWidth(minWidth);
}
