#ifndef CUSTOMTREEWIDGET_H
#define CUSTOMTREEWIDGET_H

#include <QtWidgets>

class CustomTreeWidget : public QTreeWidget
{
public:
    CustomTreeWidget(QWidget* parent = 0);

    QTreeWidgetItem* itemFromIndex(QModelIndex index);
};

#endif // CUSTOMTREEWIDGET_H
