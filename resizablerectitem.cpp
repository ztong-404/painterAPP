#include "resizablerectitem.h"
#include <QPen>
#include <QGraphicsScene>
#include <QPainter>
#include <QMimeData>
#include <QGraphicsSceneDragDropEvent>
#include <QDebug>

/*
 * @@@func: setAcceptDrops(),如果 param 为 true，则此项将接受拖放事件;否则，它对于拖放事件是透明的。默认情况下，项目不接受拖放事件
*/
ResizableRectItem::ResizableRectItem()
{
    setOwnerItem(this);
    setAcceptDrops(true);
}

int ResizableRectItem::type() const
{
    return  Type;
}

/*@@@func: adjusted(),返回一个新矩形，其中 dx1、dy1、dx2 和 dy2 分别添加到此矩形的现有坐标中。所有参数都必须是有限的 */
QRectF ResizableRectItem::boundingRect() const
{
 return selectorFrameBounds().adjusted(-pen().width(),-pen().width(),
                                       pen().width(),pen().width());
}

void ResizableRectItem::paint(QPainter *painter,
                              const QStyleOptionGraphicsItem *option,
                              QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawRect(rect());
    drawHandlesIfNecessary();
}

/*获取矩形*/
QRectF ResizableRectItem::selectorFrameBounds() const
{
    return rect();
}

/*
 * @@@func:prepareGeometryChange(), 准备要更改图形项的几何形状
*/
void ResizableRectItem::setSelectorFrameBounds(QRectF boundsRect)
{
    prepareGeometryChange();
    setRect(boundsRect);
    update();
}

/*将建议的操作设置为已接受，即将删除操作设置为建议的操作*/
void ResizableRectItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if(event->mimeData()->hasColor()){
        event->acceptProposedAction();
    }else{
        QGraphicsRectItem::dragEnterEvent(event);
    }
}


void ResizableRectItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if(event->mimeData()->hasColor()){
        setBrush(QBrush(event->mimeData()->colorData().value <QColor>())) ;
        event->acceptProposedAction();
    }else{
        QGraphicsRectItem::dropEvent(event);
    }
}

//Write
QDataStream &operator<<(QDataStream &out, const ResizableRectItem &mRect)
{

    //Frame Rect
    qreal x = mRect.selectorFrameBounds().x();
    qreal y = mRect.selectorFrameBounds().y();
    qreal width = mRect.selectorFrameBounds().width();
    qreal height = mRect.selectorFrameBounds().height();

    //Position
    qreal posX= mRect.scenePos().x();
    qreal posY = mRect.scenePos().y();

    //Pen and Brush
    //Brush Color
    QColor brushColor = mRect.brush().color();

    //Pen color and width
    QPen mPen = mRect.pen();

    out << x << y << width << height << posX << posY <<
           brushColor.red() << brushColor.green() << brushColor.blue() <<
           mPen.color().red() << mPen.color().green() << mPen.color().blue() <<
           static_cast<int>(mPen.style()) << mPen.width();

    return  out;


}


//Read
QDataStream &operator>>(QDataStream &in, ResizableRectItem &mRectItem)
{

    qreal rectX;
    qreal rectY;
    qreal rectWidth;
    qreal rectHeight;

    qreal posX;
    qreal posY;

    int brushRed;
    int brushGreen;
    int brushBlue;

    int penRed;
    int penGreen;
    int penBlue;

    int penStyle;
    int penWidth;

    in >> rectX >> rectY >> rectWidth >> rectHeight >> posX >> posY >>
            brushRed >> brushGreen >> brushBlue >>
            penRed >> penGreen >> penBlue >>
            penStyle >> penWidth;

    mRectItem.setSelectorFrameBounds(QRectF(rectX,rectY,rectWidth,rectHeight));
    mRectItem.setBrush(QBrush(QColor(brushRed,brushGreen,brushBlue)));

    QPen mPen;
    mPen.setColor(QColor(penRed,penGreen,penBlue));
    mPen.setStyle(static_cast<Qt::PenStyle>(penStyle));
    mPen.setWidth(penWidth);
    mRectItem.setPen(mPen);

    mRectItem.setPos(QPointF(posX,posY));

    return  in;
}
