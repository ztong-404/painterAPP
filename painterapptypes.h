#ifndef PAINTERAPPTYPES_H
#define PAINTERAPPTYPES_H


#include <QGraphicsItem>

//Qt中的所有此类标准图形项类都与Type的唯一值相关联
const int ResizableRectType = QGraphicsItem::UserType + 1;
const int ResizableEllipseType = QGraphicsItem::UserType + 2;
const int ResizablePixmapType = QGraphicsItem::UserType + 3;
const int ResizableStarType = QGraphicsItem::UserType + 4;
const int StrokeType = QGraphicsItem::UserType + 5;

#endif // PAINTERAPPTYPES_H
