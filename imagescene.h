#ifndef IMAGESCENE_H
#define IMAGESCENE_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QRectF>
#include "imagesection.h"
#include "imageinfo.h"

enum class ImageTool
{
  Section,
  RadialSection,
  Point
};

class ImageScene : public QGraphicsScene
{
  Q_OBJECT

  QGraphicsLineItem* crosslineHorizontal;
  QGraphicsLineItem* crosslineVertical;
  QGraphicsPixmapItem* currentImage;
  QList<QGraphicsPolygonItem*> sectionItems;
  QList<QGraphicsEllipseItem*> radSectionItems;
  QList<QGraphicsItem*> pointItems;
  ImageInfo currentInfo;
  ImageTool currentTool;

  bool drawing;
  QList<QPoint> drawnPoints;
  QList<QGraphicsItem*> drawnPreview;
  QPoint drawStartPoint;
  QGraphicsItem* previewGraphicsItem;
  void drawCrossLine(const QPointF& point);
  QRect getRect(QPoint point1, QPoint point2);

  void drawPoint(const QPoint& point);
  void drawRevert();
  void drawFinish();
  void drawAbort();
  void drawPreview(const QPointF& point);
  void deleteItem(const QPointF& point);

  QGraphicsEllipseItem* addRadialItem(const QPointF& center, qreal radius, const QColor& color=QColor(255,255,0));
  QGraphicsEllipseItem* addRadialItem(const QPointF& center, const QPointF& point, const QColor& color=QColor(255,255,0));
  QPointF getContainedPoint(QPointF point);
  void clearImage();
  void clearSections();
  void clear();
  void loadSectionsFromCurrentInfo();
  void loadFromCurrentInfo();
public:
  explicit ImageScene(QObject *parent = 0);
  void setTool(ImageTool tool);
  void setImageInfo(ImageInfo* info);
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
  virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
signals:
  void radSectionGenerated(const ImageRadialSection& section);
  void radSectionRemove(const ImageRadialSection& section);
  void pointSectionGenerated(const ImagePointSection& section);
  void pointSectionRemove(const ImagePointSection& section);
  void sectionGenerated(const ImageSection& section);
  void sectionRemove(const ImageSection& section);
public slots:
  void drawRadialSection(const ImageRadialSection& section);
  void drawRadialSections(const QList<const ImageRadialSection*>& sections);
  void drawImageSection(const ImageSection& section);
  void drawImageSections(const QList<const ImageSection*>& sections);
  void drawPointSection(const ImagePointSection& section);
  void drawPointSections(const QList<const ImagePointSection*> sections);
};

#endif // IMAGESCENE_H
