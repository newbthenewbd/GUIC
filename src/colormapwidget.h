#ifndef COLORMAPWIDGET_H
#define COLORMAPWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QLinearGradient>
#include <QGradientStops>
#include <QColor>
#include <QString>
#define TINYCOLORMAP_WITH_QT5
#include "tinycolormap.hpp"

class ColorMapWidget : public QWidget
{
    using QWidget::QWidget; //constructors
    
    private:
    QLinearGradient gradient;
    tinycolormap::ColormapType colormapType = tinycolormap::ColormapType::Heat;
    double minValue = 0.0, maxValue = 0.0;
    QString unit;
    
    public:
    QColor getColor(double value)
    {
        return tinycolormap::GetColor(value, colormapType).ConvertToQColor();
    }
    
    void setColormap(tinycolormap::ColormapType type)
    {
        colormapType = type;
        
        QGradientStops stops;
        
        for(int i = 0; i < 255; i++)
        {
            stops << QGradientStop(i / 255.0, getColor(1.0 - i / 255.0));
        }
        
        gradient.setStops(stops);
    }
    
    double getMinValue()
    {
        return minValue;
    }
    
    void setMinValue(double value)
    {
        minValue = value;
    }
    
    double getMaxValue()
    {
        return maxValue;
    }
    
    void setMaxValue(double value)
    {
        maxValue = value;
    }
    
    QString getUnit()
    {
        return unit;
    }
    
    void setUnit(QString unit)
    {
        this->unit = unit;
    }
    
    protected:
    void paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        
        int h = height()-2;
        
        gradient.setFinalStop(0, h);
        
        painter.fillRect(0, 0, 32, height(), Qt::black);
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
        
        int minWidth = 0;
        
        for(int i = 0; i < 26; i++)
        {
            QString text = QString::number((maxValue-minValue)*i/26.0, 'f', 3) + " " + unit;
            painter.drawText(56, (i+0.85)*h/26.0, text);
            if(painter.fontMetrics().boundingRect(text).width() + 56 + 2 > minWidth)
            {
                minWidth = painter.fontMetrics().boundingRect(text).width() + 56 + 2;
            }
        }
        
        this->setMinimumWidth(minWidth);
    }
};

#endif // COLORMAPWIDGET_H
