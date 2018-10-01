#include "customTreeWidget.h"

CustomTreeWidget::CustomTreeWidget(QWidget* parent):
    QTreeWidget(parent)
{

}

QTreeWidgetItem* CustomTreeWidget::itemFromIndex(QModelIndex index)
{
    return QTreeWidget::itemFromIndex(index);
}
