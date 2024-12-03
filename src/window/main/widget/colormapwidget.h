#ifndef COLORMAPWIDGET_H
#define COLORMAPWIDGET_H

#include <QWidget>
#include <QString>
#define TINYCOLORMAP_WITH_QT5
#include <ext/tinycolormap.hpp>

class ColorMapWidget : public QWidget
{
	Q_OBJECT
	
	using QWidget::QWidget; //constructors
	
	private:
	QLinearGradient gradient;
	tinycolormap::ColormapType colormapType = tinycolormap::ColormapType::Heat;
	double minValue = 0.0, maxValue = 0.0;
	int decimals = 3;
	QString unit;
	
	signals:
	void unitChanged(QString unit);
	
	public:
	void setColormap(tinycolormap::ColormapType type);
	QColor getColor(double value);
	int getDecimals();
	double getMinValue();
	double getMaxValue();
	const QString& getUnit();
	
	public slots:
	void setDecimals(int decimals);
	void setMinValue(double value);
	void setMaxValue(double value);
	void setUnit(const QString& unit);
	
	protected:
	void paintEvent(QPaintEvent* event) override;
};

#endif // COLORMAPWIDGET_H
