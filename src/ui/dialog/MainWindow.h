#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "listView/materialListedData.h"
#include "customTreeWidget.h"

namespace Ui
{
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum EN_SUPPORTED_LANG {
        EN = 0,
        JA,
        CH,

        LANG_CENTINEL
    };
    Q_ENUM(EN_SUPPORTED_LANG)

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setArguments(QString defaultPath, QString locale, QString laserType = "");
    static QString getLocaleStr(int);

private slots:
    void findLibrary();
    void loadLibrary();

    void removePreset();
    void addNewPreset();
    void finishEdit();

    void updateLibTable(QString currentTag);
//    void selectTree(QModelIndex index);

    void validate(QTreeWidgetItem* item, int col);

private:
    int  loadLibData();
    void createLibTable();
    void createTrees();

    void setData(const QString laserType, const QJsonArray& array);
    void writeDown(QJsonObject &json);
    void applyToData();
    void insertTableRow(QString laserType, MaterialListedData* data);

    void tableInitialize();

    QString      currentLaserType();
    CustomTreeWidget* currentTree();
//    void         setStringForNameTable(int row, int col, QString lang);
//    QPair<QString, QString> stringFromNameTable(QString lang);

    MaterialListedData*  dataWithDefaults(QString name, QString clas, qreal thick);

private:
    Ui::MainWindow *ui;

    QMultiMap<QString, MaterialListedData*>  mData;
    QStringList                              mTagList;
    QString                                  mLocale;
    QString                                  mUsingLaserType;
};

#endif // MAINWINDOW_H
