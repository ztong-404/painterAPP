#include "scene.h"
#include <QMimeData>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include "resizableellipseitem.h"
#include "resizablepixmapitem.h"
#include "resizablerectitem.h"
#include "resizablestaritem.h"
#include "commands.h"
#include <QDebug>

const QString MimeType = "application/com.blikoontech.painterapp";

Scene::Scene(QObject *parent) : QGraphicsScene(parent),
    tool(Cursor),
    drawing(false),
    lineGroup(nullptr),
    lastEraserCircle(nullptr),
    lastItem (nullptr),
    penColor(Qt::black),
    penWidth(2),
    penStyle(Qt::SolidLine),
    fillColor(Qt::gray),
    brushStyle(Qt::SolidPattern)
{
    horGuideLine =  addLine(-400,0,400,0,QPen(Qt::blue));
    verGuideLine = addLine(0,-400,0,400,QPen(Qt::blue));
    setSceneRect(-800,-600,1600,1200);

    undoStack = new QUndoStack(this);

}

void Scene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if(event->mimeData()->property("Key").canConvert(QMetaType::Int)){
        event->acceptProposedAction();
    }else{
        QGraphicsScene::dragMoveEvent(event);
    }
}

void Scene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if(event->mimeData()->property("Key").canConvert(QMetaType::Int)){

        int key = event->mimeData()->property("Key").toInt();

        QPen mPen;
        mPen.setColor(penColor);
        mPen.setWidth(penWidth);
        mPen.setStyle(penStyle);

        QBrush mBrush;
        mBrush.setColor(fillColor);
        mBrush.setStyle(brushStyle);


        switch (key) {
        case 10:{
            //Ellipse
            ResizableEllipseItem * ellipse = new ResizableEllipseItem();
            ellipse->setRect(0,0,80,50);
            ellipse->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
            ellipse->setBrush(mBrush);
            ellipse->setPen(mPen);

            AddCommand* addCommand = new AddCommand(ellipse,this);
            undoStack->push(addCommand);
            //addItem(ellipse);

            ellipse->setPos(event->scenePos() -QPointF((ellipse->boundingRect().width()/2),
                                                       (ellipse->boundingRect().height()/2))) ;

        }
            break;
        case 20:{
            //Qt Quick Image
            ResizablePixmapItem * pixItem = new ResizablePixmapItem(QPixmap(":/images/quick.png"));
            pixItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
            pixItem->setPen(mPen);

            //addItem(pixItem);
            AddCommand* addCommand = new AddCommand(pixItem,this);
            undoStack->push(addCommand);

            pixItem->setPos(event->scenePos() -QPointF((pixItem->boundingRect().width()/2),
                                                       (pixItem->boundingRect().height()/2))) ;
        }
            break;
        case 30:{
            //Rectangle
            ResizableRectItem * rectItem = new ResizableRectItem();
            rectItem->setRect(0,0,80,50);
            rectItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
            rectItem->setBrush(mBrush);
            rectItem->setPen(mPen);

            //addItem(rectItem);
            AddCommand* addCommand = new AddCommand(rectItem,this);
            undoStack->push(addCommand);

            rectItem->setPos(event->scenePos() -QPointF((rectItem->boundingRect().width()/2),
                                                        (rectItem->boundingRect().height()/2))) ;
        }
            break;
        case 40:{
            //Star
            ResizableStarItem * starItem = new ResizableStarItem();
            starItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
            starItem->setBrush(mBrush);
            starItem->setPen(mPen);

            // addItem(starItem);
            AddCommand* addCommand = new AddCommand(starItem,this);
            undoStack->push(addCommand);

            starItem->setPos(event->scenePos() -QPointF((starItem->boundingRect().width()/2),
                                                        (starItem->boundingRect().height()/2))) ;
        }
            break;

        }



        event->acceptProposedAction();
    }else{
        QGraphicsScene::dropEvent(event);
    }

}

/*
@@@fun:scenePos(),返回鼠标光标在场景坐标中的位置

*/
void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){

        if(tool == ToolType::Pen || tool == Eraser
                || tool ==  Rect || tool == Star || tool == Ellipse){
            startingPoint = event->scenePos();
            drawing = true;
        }else if(tool == Cursor){

            m_selectedItems.clear();
            selectedItemInitialPositions.clear();
            QGraphicsScene::mousePressEvent(event);
        }else{
            QGraphicsScene::mousePressEvent(event);
        }

    }else{
        QGraphicsScene::mousePressEvent(event);
    }
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if((event->buttons() & Qt::LeftButton) && drawing){
        if(tool == ToolType::Pen){
            drawLineTo(event->scenePos());
        }else if(tool == ToolType::Eraser){
            drawEraserAt(event->scenePos());
        }else{
            drawShapeTo(event->scenePos());
        }

    }else if((event->buttons() & Qt::LeftButton)&&(tool == Cursor)){
        if(m_selectedItems.isEmpty() && selectedItemInitialPositions.isEmpty()
                &&!selectedItems().isEmpty()){

            m_selectedItems = selectedItems();

            foreach (QGraphicsItem * item, m_selectedItems) {
                selectedItemInitialPositions.append(item->scenePos());
            }
        }
        QGraphicsScene::mouseMoveEvent(event);

    }else{
        QGraphicsScene::mouseMoveEvent(event);
    }

}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if((event->button() == Qt::LeftButton) && drawing){


        QPen mPen;
        mPen.setColor(penColor);
        mPen.setWidth(penWidth);
        mPen.setStyle(penStyle);

        QBrush mBrush;
        mBrush.setColor(fillColor);
        mBrush.setStyle(brushStyle);



        if(lastItem && ((tool == Rect)
                        || (tool == Ellipse)
                        || (tool == Star))){
            removeItem(lastItem);
            //We are responsible to release this piece of memory
            delete lastItem;
        }

        if(tool == ToolType::Pen){
            lineGroup = nullptr;
            drawing = false;
        }

        if(tool == ToolType::Eraser){
            removeItem(lastEraserCircle);
            delete lastEraserCircle;
            lastEraserCircle = nullptr;
            drawing = false;
        }

        if(tool == Rect){

            ResizableRectItem * mRect = new ResizableRectItem();
            mRect->setRect(QRectF(startingPoint,event->scenePos()).normalized());
            mRect->setBrush(mBrush);
            mRect->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
            mRect->setPen(mPen);

            //addItem(mRect);
            AddCommand* addCommand = new AddCommand(mRect,this);
            undoStack->push(addCommand);

            lastItem = nullptr;
            drawing = false;
        }

        if(tool == Ellipse){

            ResizableEllipseItem * ellipseItem = new ResizableEllipseItem();
            ellipseItem->setRect(QRectF(startingPoint,event->scenePos()).normalized());
            ellipseItem->setBrush(mBrush);
            ellipseItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
            ellipseItem->setPen(mPen);

            //addItem(ellipseItem);
            AddCommand* addCommand = new AddCommand(ellipseItem,this);
            undoStack->push(addCommand);

            lastItem = nullptr;
            drawing = false;
        }

        if(tool == Star){

            ResizableStarItem * starItem = new ResizableStarItem();
            starItem->setRect(QRectF(startingPoint,event->scenePos()).normalized());
            starItem->setBrush(mBrush);
            starItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
            starItem->setPen(mPen);

            //addItem(starItem);
            AddCommand* addCommand = new AddCommand(starItem,this);
            undoStack->push(addCommand);

            lastItem = nullptr;
            drawing = false;
        }

    }else if((event->button() == Qt::LeftButton) && (tool == Cursor)){

        for(int i = 0 ; i < m_selectedItems.count() ; i++){

            QGraphicsItem * currentItem = m_selectedItems[i];


            if(!(selectedItemInitialPositions[i] == currentItem->scenePos())){
                MoveCommand * moveCommand = new MoveCommand(currentItem,this,
                                                            selectedItemInitialPositions[i],
                                                            currentItem->scenePos());
                undoStack->push(moveCommand);
            }
        }

        QGraphicsScene::mouseReleaseEvent(event);


    }else{
        QGraphicsScene::mouseReleaseEvent(event);
    }

}

void Scene::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete){
        if(selectedItems().count()  > 0){
            QGraphicsItem * itemToRemove = selectedItems()[0];
            RemoveCommand * removeCommand = new RemoveCommand(itemToRemove,this);
            undoStack->push(removeCommand);
            //            removeItem(itemToRemove);
            //            delete itemToRemove;
        }
    }
    QGraphicsScene::keyPressEvent(event);
}

void Scene::drawLineTo(const QPointF &endPoint)
{
    if(!lineGroup){
        lineGroup = new StrokeItem();
        lineGroup->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

        AddCommand * addCommand = new AddCommand(lineGroup,this);
        undoStack->push(addCommand);
        //addItem(lineGroup);


        lastPenPoint = startingPoint;
    }

    QGraphicsLineItem  *localLine = new QGraphicsLineItem(QLineF(lastPenPoint,endPoint));
    QPen mPen;
    mPen.setWidth(penWidth);
    mPen.setColor(penColor);
    localLine->setPen(mPen);
    lineGroup->addToGroup(localLine);

    lastPenPoint = endPoint;
}

void Scene::drawEraserAt(const QPointF &endPoint)
{
    if(!lastEraserCircle){
        lastEraserCircle = addEllipse(0,0,50,50);
    }
    lastEraserCircle->setPos(endPoint - QPointF(lastEraserCircle->boundingRect().width()/2,
                                                lastEraserCircle->boundingRect().height()/2));
    eraseStrokesUnder(lastEraserCircle);

}

void Scene::eraseStrokesUnder(QGraphicsEllipseItem *item)
{
    QList<QGraphicsItem *> itemsToRemove = item->collidingItems();
    QList<QGraphicsItemGroup *> groupItems;

    foreach (QGraphicsItem * myItem, itemsToRemove) {

        QGraphicsItemGroup * group = dynamic_cast<QGraphicsItemGroup *>(myItem);
        if(group){
            groupItems.append(group);
        }

        //Cast to graphicsLineItem
        QGraphicsLineItem * line = dynamic_cast<QGraphicsLineItem *>(myItem);
        if(line && (line != horGuideLine) && (line != verGuideLine)){
            removeItem(line);
            delete line;
        }

    }

    //Remove group items that don't have any children.
    foreach (QGraphicsItemGroup * group, groupItems) {
        if(group->childItems().count() == 0){
            qDebug() << "Group item has no child. Removing it";
            removeItem(group);
            delete group;
        }
    }
}

void Scene::drawShapeTo(const QPointF &endPoint)
{
    if(lastItem){
        removeItem(lastItem);
        delete lastItem;
    }

    QPen mPen;
    mPen.setColor(penColor);
    mPen.setWidth(penWidth);
    mPen.setStyle(penStyle);

    QBrush mBrush;
    mBrush.setColor(fillColor);
    mBrush.setStyle(brushStyle);

    QRectF itemRect(startingPoint,endPoint);

    if(tool == Rect){
        ResizableRectItem * mRect = new ResizableRectItem();
        mRect->setRect(itemRect.normalized());
        mRect->setBrush(mBrush);
        mRect->setPen(mPen);
        addItem(mRect);
        mRect->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
        lastItem = mRect;
    }

    if(tool == Ellipse){
        ResizableEllipseItem * ellipseItem = new ResizableEllipseItem();
        ellipseItem->setRect(itemRect.normalized());
        ellipseItem->setBrush(mBrush);
        ellipseItem->setPen(mPen);
        addItem(ellipseItem);
        ellipseItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
        lastItem = ellipseItem;
    }
    if(tool == Star){
        ResizableStarItem * starItem = new ResizableStarItem();
        starItem->setRect(itemRect.normalized());
        starItem->setBrush(mBrush);
        starItem->setPen(mPen);
        addItem(starItem);
        starItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
        lastItem = starItem;
    }
}

/*
@@@func: moveby(),通过水平移动 dx 点，垂直移动 dy 点。此函数等效于调用setPos(pos()+QPointF(dx,dy))
@@@func: additem(), 将项目及其所有子项添加或移动到此场景中。此场景将获取项目的所有权
*/
void Scene::readItemsFromDataStream(QDataStream &in ,bool copyPaste)
{
    clearSelection();

    qint32 itemType;

    while (!in.atEnd()) {
        in >> itemType;

        switch (itemType) {

        case ResizableRectType :{
            ResizableRectItem * rectItem = new ResizableRectItem();
            in >> *rectItem;
            rectItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
            addItem(rectItem);
            if(copyPaste){
                rectItem->moveBy(10,10);
                rectItem->setSelected(true);
            }
            break;
        }

        case ResizableEllipseType :{

            ResizableEllipseItem *ellipseItem = new ResizableEllipseItem();
            in >> *ellipseItem;
            ellipseItem->setFlag(QGraphicsItem::ItemIsMovable);
            ellipseItem->setFlag(QGraphicsItem::ItemIsSelectable);
            addItem(ellipseItem);
            if(copyPaste){
                ellipseItem->moveBy(10,10);
                ellipseItem->setSelected(true);
            }
            break;
        }

        case ResizablePixmapType :{
            ResizablePixmapItem *pixmapItem = new ResizablePixmapItem(QPixmap());
            in >> *pixmapItem;
            pixmapItem->setFlag(QGraphicsItem::ItemIsMovable);
            pixmapItem->setFlag(QGraphicsItem::ItemIsSelectable);
            addItem(pixmapItem);
            if(copyPaste){
                pixmapItem->moveBy(10,10);
                pixmapItem->setSelected(true);
            }
            break;
        }

        case ResizableStarType :{

            ResizableStarItem *starItem = new ResizableStarItem();
            in >> *starItem;
            starItem->setFlag(QGraphicsItem::ItemIsMovable);
            starItem->setFlag(QGraphicsItem::ItemIsSelectable);
            addItem(starItem);
            if(copyPaste){
                starItem->moveBy(10,10);
                starItem->setSelected(true);
            }
            break;
        }

        case StrokeType :{
            StrokeItem *strokeItem = new StrokeItem();
            in >> *strokeItem;
            strokeItem->setFlag(QGraphicsItem::ItemIsMovable);
            strokeItem->setFlag(QGraphicsItem::ItemIsSelectable);
            addItem(strokeItem);
            if(copyPaste){
                strokeItem->moveBy(100,100);
                strokeItem->setSelected(true);
            }
            break;
        }

        }

    }

}

/*从QList容器寻找到items类，并进行写入文件序列化的数据*/
void Scene::writeItemsToDataStream(QDataStream &out, const QList<QGraphicsItem *> &items)
{
    foreach (QGraphicsItem * item, items) {

        qint32 type = static_cast<qint32>(item->type());
        out << type;

        switch (type) {

        case ResizableRectType :
            out << *static_cast<ResizableRectItem *>(item);
            break;
        case ResizableEllipseType :
            out << *static_cast<ResizableEllipseItem *>(item);
            break;
        case ResizablePixmapType :
            out << *static_cast<ResizablePixmapItem *>(item);
            break;
        case ResizableStarType :
            out << *static_cast<ResizableStarItem *>(item);
            break;
        case StrokeType :
            out << *static_cast<StrokeItem *>(item);
            break;

        }

    }

}

Qt::BrushStyle Scene::getBrushStyle() const
{
    return brushStyle;
}

void Scene::setBrushStyle(const Qt::BrushStyle &value)
{
    brushStyle = value;
}

/*
@@@func: writeItemsToDataStream(),
*/
void Scene::copy()
{
    QByteArray mByteArray;
    QDataStream out(&mByteArray, QIODevice::WriteOnly);
    writeItemsToDataStream(out,selectedItems());
    QMimeData * mimeData = new QMimeData;
    mimeData->setData(MimeType,mByteArray);
    QClipboard * clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimeData);
}


void Scene::cut()
{
    QByteArray mByteArray;
    QDataStream out(&mByteArray, QIODevice::WriteOnly);

    QList<QGraphicsItem* > itemList = selectedItems();

    writeItemsToDataStream(out,itemList);
    QMimeData * mimeData = new QMimeData;
    mimeData->setData(MimeType,mByteArray);
    QClipboard * clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimeData);

    //Remove items
    foreach (QGraphicsItem * item, itemList) {
        removeItem(item);
    }
    qDeleteAll(itemList);

}

void Scene::paste()
{
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    if(!mimeData)
        return;

    if(mimeData->hasFormat(MimeType)){
        QByteArray mByteArray = mimeData->data(MimeType);
        QDataStream in(&mByteArray, QIODevice::ReadOnly);

        readItemsFromDataStream(in);

    }

}

void Scene::saveScene(QString &filename)
{
    QFile file(filename);

    if(!file.open(QIODevice::WriteOnly))
        return;

    QDataStream dataStream(&file);
    writeItemsToDataStream(dataStream,items());

    file.close();

}

void Scene::loadScene(QString &filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        return;
    QDataStream dataStream(&file);
    readItemsFromDataStream(dataStream,false);
    file.close();
}

void Scene::undo()
{
    undoStack->undo();
}

void Scene::redo()
{
    undoStack->redo();
}

QColor Scene::getFillColor() const
{
    return fillColor;
}

void Scene::setFillColor(const QColor &value)
{
    fillColor = value;
}

Qt::PenStyle Scene::getPenStyle() const
{
    return penStyle;
}

void Scene::setPenStyle(const Qt::PenStyle &value)
{
    penStyle = value;
}

int Scene::getPenWidth() const
{
    return penWidth;
}

void Scene::setPenWidth(int value)
{
    penWidth = value;
}

QColor Scene::getPenColor() const
{
    return penColor;
}

void Scene::setPenColor(const QColor &value)
{
    penColor = value;
}

Scene::ToolType Scene::getTool() const
{
    return tool;
}

void Scene::setTool(const ToolType &value)
{
    tool = value;
}

void Scene::addImageItem(const QString &path)
{
    ResizablePixmapItem * pixItem = new ResizablePixmapItem(QPixmap(path));
    pixItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    addItem(pixItem);

    pixItem->setPos(QPointF(0,0) - QPointF(pixItem->boundingRect().width()/2,
                                           pixItem->boundingRect().height()/2));
}
