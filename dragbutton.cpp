#include "dragbutton.h"
#include <QMouseEvent>
#include <QApplication>
#include <QMimeData>
#include <QDrag>

DragButton::DragButton(QWidget *parent) : QPushButton(parent),
    buttonColor(Qt::gray)
{

}

QColor DragButton::getButtonColor() const
{
    return buttonColor;
}

void DragButton::setButtonColor(const QColor &value)
{
    buttonColor = value;
}

/*记录鼠标事件产生时的位置*/
void DragButton::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        lastPos = event->pos();
    }
    QPushButton::mousePressEvent(event);
}

/*@@@func: manhattanLength(),传统上称为“曼哈顿长度”的向量，从原点到点
 *@@@func: startDragDistance(),通过鼠标移动的距离来判断是否启动拖放
 * int distance,记录光标从点击到事件产生移动的距离
*/
void DragButton::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton){

        int distance = (event->pos() - lastPos).manhattanLength();

        if( distance >= QApplication::startDragDistance()){
            QDrag* drag = new QDrag(this);
            QMimeData *mimeData = new QMimeData;
            mimeData->setColorData(buttonColor);
            QPixmap pix(20, 20);
            pix.fill(buttonColor);
            drag->setPixmap(pix);
            drag->setMimeData(mimeData);
            drag->exec();
        }
    }
    QPushButton::mouseMoveEvent(event);
}
