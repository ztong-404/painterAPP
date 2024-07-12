#include "handleitem.h"
#include "resizablehandlerect.h"

#include <QGraphicsSceneMouseEvent>

/*初始化位置并设立flag: ItemIsMovable
    position 是枚举参数*/
HandleItem::HandleItem(Position position) :
    handlePosition(position)
{
    setFlag(QGraphicsItem::ItemIsMovable);

}

/*
 * @@@func: parentItem(), 返回指向此项的父项的指针。如果此项没有父项，则返回 nullptr
 * @@@func: normalized(), 返回一个规范化的矩形;即，具有非负宽度和高度的矩形
*/
void HandleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    switch (handlePosition) {

    case TopLeft:
    {

        ResizableHandleRect * rectItem = dynamic_cast<ResizableHandleRect *>( parentItem());
        if(rectItem){

            QRectF boundingFrameRect = rectItem->selectorFrameBounds();

            boundingFrameRect.setTop(event->pos().y() + 6);
            boundingFrameRect.setLeft(event->pos().x() +6);

            rectItem->setSelectorFrameBounds(boundingFrameRect.normalized());
        }
    }
        break;

    case TopRight:
    {

        ResizableHandleRect * rectItem = dynamic_cast<ResizableHandleRect *>( parentItem());
        if(rectItem){

            QRectF boundingFrameRect = rectItem->selectorFrameBounds();

            boundingFrameRect.setTop(event->pos().y() +6);
            boundingFrameRect.setRight(event->pos().x() -6);

            rectItem->setSelectorFrameBounds(boundingFrameRect.normalized());
        }
    }
    break;


    case BottomRight:
    {
        ResizableHandleRect * rectItem = dynamic_cast<ResizableHandleRect *>( parentItem());
        if(rectItem){

            QRectF boundingFrameRect = rectItem->selectorFrameBounds();

            boundingFrameRect.setRight(event->pos().x() - 6);
            boundingFrameRect.setBottom(event->pos().y() -6);

            rectItem->setSelectorFrameBounds(boundingFrameRect.normalized());

        }

    }
        break;

    case BottomLeft:
    {
        ResizableHandleRect * rectItem = dynamic_cast<ResizableHandleRect *>( parentItem());
        if(rectItem){

            QRectF boundingFrameRect = rectItem->selectorFrameBounds();

            boundingFrameRect.setBottom(event->pos().y() -6);
            boundingFrameRect.setLeft(event->pos().x() + 6);

            rectItem->setSelectorFrameBounds(boundingFrameRect.normalized());
        }

    }
        break;

    }

}
