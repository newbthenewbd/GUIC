#ifndef COLORPICKERBUTTON_H
#define COLORPICKERBUTTON_H

#include <QToolButton>
#include <QColorDialog>
#include <QPainter>

class ColorPickerButton : public QToolButton
{
    using QToolButton::QToolButton; //constructors
    
    private:
    QColor color = Qt::white;
    
    public:
    void setColor(QColor color)
    {
        if(color.isValid())
        {
            this->color = color;
            repaint();
        }
    }
    
    QColor getColor()
    {
        return color;
    }
    
    protected:
    void paintEvent(QPaintEvent* event) override
    {
        QToolButton::paintEvent(event);
        
        QPainter painter(this);
        
        painter.fillRect(width()*0.25 + 0.5 - 1.0, height()*0.25 + 0.5 - 1.0, width()*0.5 + 0.5 + 2.0, height()*0.5 + 0.5 + 2.0, Qt::lightGray);
        painter.fillRect(width()*0.25 + 0.5, height()*0.25 + 0.5, width()*0.5 + 0.5, height()*0.5 + 0.5, color);
    }
    
    void mouseReleaseEvent(QMouseEvent* event) override
    {
        QToolButton::mouseReleaseEvent(event);
        setColor(QColorDialog::getColor());
    }    
};


#endif // COLORPICKERBUTTON_H
