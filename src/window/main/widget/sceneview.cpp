#include "sceneview.h"
#include <QTransform>
#include <QPainter>
#include <cmath>

double SceneView::getScale()
{
	return transform().m11(); //Assumption: no rotations and m22 is equal to m11
}

void SceneView::setScale(double scale)
{
	QTransform t = transform();
	scale = scale > 99.99 ? 99.99 : scale;
	scale = scale < 0.25 ? 0.25 : scale;
	setTransform(QTransform(scale, t.m12(), t.m13(), t.m21(), scale, t.m23(), t.m31(), t.m32(), t.m33()));
	emit scaledPercent(scale * 100.0);
}

int SceneView::getScalePercent()
{
	return (int) (transform().m11() * 100.0);
}

void SceneView::setScalePercent(int scalePercent)
{
	setScale(scalePercent / 100.0);
}

void SceneView::resetScale()
{
	setScale(1.0);
}

void SceneView::wheelEvent(QWheelEvent* event)
{
	if(event->modifiers() & Qt::ControlModifier)
	{
		int angle = event->angleDelta().y();
		if(angle >= 5 || angle <= -5)
		{
			int scalePercent = getScale() * (angle >= 5 ? 1.1 : 1.0/1.1) * 100.0 + /*cheap ceil*/ (double) (angle >= 5) * 0.9;
			setScalePercent(scalePercent);
		}
	}
	else
	{
		QGraphicsView::wheelEvent(event);
	}
}

void SceneView::drawForeground(QPainter* painter, const QRectF& rect)
{
	static QPen rulerPen = QPen(QBrush(QColor(255, 255, 0, 180)), 24.0, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
	static QPen pointsPen = QPen(QBrush(QColor(255, 0, 255)), 2.0, Qt::SolidLine, Qt::RoundCap);
	rulerPen.setCosmetic(true);
	pointsPen.setCosmetic(true);
	
	painter->setRenderHint(QPainter::Antialiasing, true);
	
	painter->setPen(rulerPen);
	painter->drawLine(ruler);
	
	painter->setPen(pointsPen);
	for(int i = 1; i < points.count(); i++) {
		painter->drawLine(points[i-1], points[i]);
	}
}