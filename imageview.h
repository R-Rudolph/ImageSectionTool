#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPointF>

class ImageView : public QGraphicsView
{
  Q_OBJECT
  qreal zoomFactor = 1.0;
  QPointF sceneHook;

  void moveSceneToScreenPos(QPointF scenePos, QPointF screenPos);
public:
  explicit ImageView(QWidget *parent = 0);
protected:
  void wheelEvent(QWheelEvent* event);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
signals:

public slots:
};

#endif // IMAGEVIEW_H
