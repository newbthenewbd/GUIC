#ifndef COLORMAPWIDGET_H
#define COLORMAPWIDGET_H

#include <QWidget>
#include <QString>
#define TINYCOLORMAP_WITH_QT5
#include <tinycolormap.hpp>

class ColorMapWidget : public QWidget
{
	Q_OBJECT
	
	using QWidget::QWidget; //constructors
	
	private:
	QLinearGradient gradient;
	tinycolormap::ColormapType colormapType = tinycolormap::ColormapType::Heat;
	double minValue = 0.0, maxValue = 0.0;
	QString unit;
	
	signals:
	void unitChanged(QString unit);
	
	public:
	void setColormap(tinycolormap::ColormapType type);
	QColor getColor(double value);
	double getMinValue();
	double getMaxValue();
	QString getUnit();
	
	public slots:
	void setMinValue(double value);
	void setMaxValue(double value);
	void setUnit(QString unit);
	
	protected:
	void paintEvent(QPaintEvent* event) override;
};

#endif // COLORMAPWIDGET_H
