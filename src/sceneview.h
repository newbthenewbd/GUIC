#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#define W_NO_PROPERTY_MACRO
#include <wobjectdefs.h>
#include <QGraphicsView>
#include <QWheelEvent>

class SceneView : public QGraphicsView
{
	//Q_OBJECT
	W_OBJECT(SceneView)
	
	using QGraphicsView::QGraphicsView; //constructors
	
	signals:
	void scaledPercent(int percent) W_SIGNAL(scaledPercent, percent)
	
	public:
	double getScale();
	int getScalePercent();
	
	public slots:
	void setScale(double scale);
	W_SLOT(setScale)
	void setScalePercent(int scalePercent);
	W_SLOT(setScalePercent)
	void resetScale();
	W_SLOT(resetScale)
	
	protected:
	void wheelEvent(QWheelEvent* event) override;
};

#endif // SCENEVIEW_H
