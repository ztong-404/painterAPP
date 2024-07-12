#include "colorlistwidget.h"
#include <QDrag>
#include <QMimeData>
#include <QDebug>

/*@@@func: setSelectionMode(), 此属性控制用户是否可以选择一个或多个项目，以及在多项项目选择中，所选内容是否必须是连续的项目范围
@@@func: setViewMode设置视图模式将根据所选移动启用或禁用拖放。对于 ListMode，默认移动为 Static（禁用拖放）;对于 IconMode，默认移动为 Free（启用拖放）
*/

ColorListWidget::ColorListWidget(QWidget *parent) : QListWidget(parent)
{
    setMinimumWidth(200);
    setMaximumWidth(300);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setViewMode(QListView::IconMode);

}

void ColorListWidget::startDrag(Qt::DropActions supportedActions)
{
    QList<QListWidgetItem *> items = selectedItems();
    if(items.count() > 0){

        QDrag* drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        QColor color(items[0]->text());

        mimeData->setColorData(color);

        QPixmap pix(20, 20);
        pix.fill(color);
        drag->setPixmap(pix);
        drag->setMimeData(mimeData);
        drag->exec(supportedActions);

//        if(drag->exec() == Qt::IgnoreAction){
//            qDebug() << "Drag ignored";
//        }
    }

}
