#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

class SceneView : public QGraphicsView
{
	Q_OBJECT
	
	using QGraphicsView::QGraphicsView; //constructors
	
	signals:
	void scaledPercent(int percent);
	
	public:
	QLineF ruler;
	QVector<QPointF> points;
	QVector<QPointF> savedPoints;
	double getScale();
	int getScalePercent();
	
	public slots:
	void setScale(double scale);
	void setScalePercent(int scalePercent);
	void resetScale();
	
	protected:
	void wheelEvent(QWheelEvent* event) override;
	void drawForeground(QPainter* painter, const QRectF& rect) override;
};

#endif // SCENEVIEW_H
