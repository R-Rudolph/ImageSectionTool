#include "imageview.h"

void ImageView::moveSceneToScreenPos(QPointF scenePos, QPointF screenPos)
{
  QPointF p1 = mapToScene(0,0);
  QPointF p2 = mapToScene(this->viewport()->width(),this->viewport()->height());
  qreal factor = ( p2.x() - p1.x() + p2.y() - p1.y() )/(this->viewport()->width() + this->viewport()->height()+2);
  QRectF visibleArea(scenePos.x() - factor*screenPos.x(),
                     scenePos.y() - factor*screenPos.y(),
                     factor*this->viewport()->width(),
                     factor*this->viewport()->height()
                     );
  this->ensureVisible(visibleArea,0,0);
  this->ensureVisible(visibleArea,0,0);
  this->update();
  this->viewport()->update();
}

ImageView::ImageView(QWidget *parent) : QGraphicsView(parent)
{
  setMouseTracking(true);
}

void ImageView::wheelEvent(QWheelEvent *event)
{
  qreal zoom = 1.0 + 0.0008*event->delta();
  zoomFactor *= zoom;
  QPointF point = mapToScene(event->pos());
  scale(zoom,zoom);
  moveSceneToScreenPos(point,event->pos());
}

void ImageView::mousePressEvent(QMouseEvent *event)
{
  QGraphicsView::mousePressEvent(event);
  if(event->buttons() == Qt::RightButton || event->buttons() == Qt::MiddleButton)
  {
    sceneHook = mapToScene(event->pos());
  }
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
  QGraphicsView::mouseMoveEvent(event);
  if(event->buttons() == Qt::RightButton || event->buttons() == Qt::MiddleButton)
  {
    moveSceneToScreenPos(sceneHook,event->pos());
  }
}

