#include "imagescene.h"
#include  <QtMath>

void ImageScene::drawCrossLine(const QPointF &point)
{
  if(sceneRect().contains(point))
  {
    crosslineHorizontal = addLine(sceneRect().left(),
                                  point.y(),
                                  sceneRect().right(),
                                  point.y(),
                                  QPen(QBrush(Qt::blue),1,Qt::DashLine));

    crosslineVertical = addLine(point.x(),
                                sceneRect().top(),
                                point.x(),
                                sceneRect().bottom(),
                                QPen(QBrush(Qt::blue),1,Qt::DashLine));
  }
}

QRect ImageScene::getRect(QPoint point1, QPoint point2)
{
  return QRect(QPoint(qMin(point1.x(),point2.x()),qMin(point1.y(),point2.y())),
               QPoint(qMax(point1.x(),point2.x()),qMax(point1.y(),point2.y())));
}

void ImageScene::drawPoint(const QPoint &point)
{
  if(drawing)
  {
    switch(currentTool)
    {
      case ImageTool::Section:
        drawnPreview.append(addLine(QLineF(drawStartPoint,point),QPen(QBrush(QColor(255,0,255)),7)));
        drawStartPoint = point;
        drawnPoints.append(point);
        break;
      case ImageTool::RadialSection:
        if(drawnPreview.size()>0)
        {
          delete drawnPreview.last();
          drawnPreview.removeLast();
        }
        drawnPreview.append(addRadialItem(drawStartPoint,point));
        drawnPoints.append(point);
        drawFinish();
        break;
      default:
        break;
    }
  }
  else
  {
    drawing = true;
    drawStartPoint = point;
    drawnPoints.append(point);
    if(currentTool==ImageTool::Point)
      drawFinish();
  }
}

void ImageScene::drawRevert()
{
  if(drawnPoints.size()<=1)
    drawAbort();
  else
  {
    switch(currentTool)
    {
      case ImageTool::Section:
        delete drawnPreview.last();
        drawnPreview.removeLast();
        drawnPoints.removeLast();
        drawStartPoint = drawnPoints.last();
        break;
      case ImageTool::RadialSection:
        drawAbort();
        break;
      case ImageTool::Point:
        break;
    }

  }
}

void ImageScene::drawFinish()
{
  if(drawing)
  {
    switch(currentTool)
    {
      case ImageTool::Section:
        if(drawnPoints.size()>2)
        {
          QPolygon polygon(drawnPoints.toVector());
          ImageSection section(polygon,0);
          emit sectionGenerated(section);
          drawImageSection(section);
          currentInfo.addImageSection(section);
        }
        break;
      case ImageTool::RadialSection:
        if(drawnPoints.size()==2)
        {
          ImageRadialSection sec(drawnPoints[0],drawnPoints[1]);
          emit radSectionGenerated(sec);
          drawRadialSection(sec);
          currentInfo.addImageRadialSection(sec);
        }
        break;
      case ImageTool::Point:
        if(drawnPoints.size()>0)
        {
          ImagePointSection sec(drawnPoints.first());
          emit pointSectionGenerated(sec);
          drawPointSection(sec);
          currentInfo.addImagePointSection(sec);
        }
    }

    drawAbort();
  }
}

void ImageScene::drawAbort()
{
  drawing = false;
  delete previewGraphicsItem;
  previewGraphicsItem = nullptr;
  foreach(QGraphicsItem* item, drawnPreview)
    delete item;
  drawnPreview.clear();
  drawnPoints.clear();
}

void ImageScene::drawPreview(const QPointF& point)
{
  switch(currentTool)
  {
    case ImageTool::Section:
      previewGraphicsItem = addLine(QLineF(drawStartPoint,point),QPen(QBrush(QColor(255,0,255)),7));
      break;
    case ImageTool::RadialSection:
      previewGraphicsItem = addRadialItem(drawStartPoint,point);
      break;
    default:
      break;
  }
}

void ImageScene::deleteItem(const QPointF &point)
{
  QList<QGraphicsItem*> posItems = items(point,Qt::ContainsItemShape);
  for(int i=0;i<posItems.size();i++)
  {
    switch(currentTool)
    {
      case ImageTool::Section:
      {
        QGraphicsPolygonItem* rItem = (QGraphicsPolygonItem*) posItems[i];
        if(sectionItems.contains(rItem))
        {
          ImageSection section(rItem->polygon().toPolygon(),0);
          currentInfo.removeImageSection(section);
          delete rItem;
          sectionItems.removeAll(rItem);
          emit sectionRemove(section);
          return;
        }
        break;
      }
      case ImageTool::RadialSection:
      {
        QGraphicsEllipseItem* rItem = (QGraphicsEllipseItem*) posItems[i];
        if(radSectionItems.contains(rItem))
        {
          ImageRadialSection section(rItem->boundingRect().center(),0.0);
          currentInfo.removeImageRadialSection(section);
          delete rItem;
          radSectionItems.removeAll(rItem);
          emit radSectionRemove(section);
          return;
        }
        break;
      }
      case ImageTool::Point:
      {
        QGraphicsItem* rItem = (QGraphicsItem*) posItems[i];
        if(pointItems.contains(rItem))
        {
          ImagePointSection section(rItem->boundingRect().center().toPoint());
          currentInfo.removeImagePointSection(section);
          delete rItem;
          pointItems.removeAll(rItem);
          emit pointSectionRemove(section);
          return;
        }
        break;
      }
    }

  }
}

QGraphicsEllipseItem *ImageScene::addRadialItem(const QPointF &center, qreal radius, const QColor& color)
{
  QRectF rect(center - QPointF(radius,radius),QSizeF(radius*2,radius*2));
  return addEllipse(rect,QPen(color,-1),QBrush(color,Qt::SolidPattern));
}

QGraphicsEllipseItem *ImageScene::addRadialItem(const QPointF &center, const QPointF &point, const QColor& color)
{
  QPointF diff = center -point;
  return addRadialItem(center,qSqrt(QPointF::dotProduct(diff,diff)),color);
}

QPointF ImageScene::getContainedPoint(QPointF point)
{
  if(point.x()<currentImage->boundingRect().left())
    point.setX(currentImage->boundingRect().left());
  if(point.x()>currentImage->boundingRect().right())
    point.setX(currentImage->boundingRect().right());
  if(point.y()>currentImage->boundingRect().bottom())
    point.setY(currentImage->boundingRect().bottom());
  if(point.y()<currentImage->boundingRect().top())
    point.setY(currentImage->boundingRect().top());
  return point;
}

void ImageScene::clearImage()
{
  delete currentImage;
  currentImage = nullptr;
  setSceneRect(0,0,0,0);
}

void ImageScene::clearSections()
{
  foreach(QGraphicsPolygonItem* item, sectionItems)
    delete item;
  sectionItems.clear();
  foreach(QGraphicsEllipseItem* item, radSectionItems)
    delete item;
  radSectionItems.clear();
  foreach(QGraphicsItem* item, pointItems)
    delete item;
  pointItems.clear();
}

void ImageScene::clear()
{
  clearImage();
  clearSections();
}

void ImageScene::loadSectionsFromCurrentInfo()
{
  clearSections();
  drawImageSections(currentInfo.getSections());
  drawRadialSections(currentInfo.getRadialSections());
  drawPointSections(currentInfo.getPointSections());
}

void ImageScene::loadFromCurrentInfo()
{
  clearImage();
  if(!currentInfo.getFilepath().isEmpty())
  {
    QPixmap pixmap(currentInfo.getFilepath());
    if(!pixmap.isNull())
    {
      currentImage = addPixmap(pixmap);
      QRectF rect = currentImage->boundingRect();
      rect.setSize(rect.size()+QSizeF(50,50));
      rect.moveCenter(currentImage->boundingRect().center());
      setSceneRect(rect);
    }
    loadSectionsFromCurrentInfo();
  }
}

ImageScene::ImageScene(QObject *parent) : QGraphicsScene(parent)
{
  setSceneRect(0,0,0,0);
  crosslineHorizontal = nullptr;
  crosslineVertical = nullptr;
  currentImage = nullptr;
  previewGraphicsItem = nullptr;
  drawing = false;
}

void ImageScene::setTool(ImageTool tool)
{
  drawAbort();
  currentTool = tool;
  loadSectionsFromCurrentInfo();
}

void ImageScene::setImageInfo(ImageInfo *info)
{
  if(info!=nullptr)
  {
    currentInfo = *info;
    loadFromCurrentInfo();
  }
}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
  if(mouseEvent->button()==Qt::LeftButton)
    drawPoint(getContainedPoint(mouseEvent->scenePos()).toPoint());
}

void ImageScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
  delete crosslineHorizontal;
  delete crosslineVertical;
  delete previewGraphicsItem;
  crosslineHorizontal = nullptr;
  crosslineVertical = nullptr;
  previewGraphicsItem = nullptr;
  if(drawing)
  {
    if(drawStartPoint!=mouseEvent->scenePos().toPoint())
      drawPreview(getContainedPoint(mouseEvent->scenePos()));
  }
  else
  {
    drawCrossLine(mouseEvent->scenePos());
  }
  mouseEvent->accept();
}

void ImageScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{

}

void ImageScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
  if(mouseEvent->button()==Qt::LeftButton && drawing)
  {
    drawFinish();
  }
  else if(mouseEvent->button()==Qt::RightButton)
  {
    if(drawing)
    {
      drawRevert();
    }
    else
    {
      deleteItem(mouseEvent->scenePos());
    }
  }
}

void ImageScene::drawRadialSection(const ImageRadialSection &section)
{
  radSectionItems.append(addRadialItem(section.getCenter(),section.getRadius()));
  if(currentTool!=ImageTool::RadialSection)
    radSectionItems.last()->setOpacity(.5);;
}

void ImageScene::drawRadialSections(const QList<const ImageRadialSection *> &sections)
{
  foreach(const ImageRadialSection* section, sections)
    drawRadialSection(*section);
}

void ImageScene::drawImageSection(const ImageSection &section)
{
  QGraphicsPolygonItem* item = addPolygon(section.getPolygon(),QPen(QBrush(QColor(255,0,255)),7));
  if(currentTool!=ImageTool::Section)
    item->setOpacity(.5);
  sectionItems.append(item);
}

void ImageScene::drawImageSections(const QList<const ImageSection *> &sections)
{
  for(int i=0;i<sections.size();i++)
    drawImageSection(*(sections[i]));
}

void ImageScene::drawPointSection(const ImagePointSection &section)
{
  QGraphicsItem* item = addRadialItem(section.getCenter(),2.5,QColor(255,0,0));
  if(currentTool!=ImageTool::Point)
    item->setOpacity(.5);
  pointItems.append(item);
}

void ImageScene::drawPointSections(const QList<const ImagePointSection *> sections)
{
  foreach(const ImagePointSection* section, sections)
    drawPointSection(*section);
}
