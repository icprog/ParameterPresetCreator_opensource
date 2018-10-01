#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "listView/materialListedData.h"
#include "customTreeWidget.h"
#include "addDataDialog.h"

#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ClassCombo->addItem("All");

    connect(ui->FindLibraryButton,  SIGNAL(clicked()), this, SLOT(findLibrary()));
    connect(ui->LoadLibraryButton,  SIGNAL(clicked()), this, SLOT(loadLibrary()));
    connect(ui->AddPresetButton,    SIGNAL(clicked()), this, SLOT(addNewPreset()));
    connect(ui->RemoveButton,       SIGNAL(clicked()), this, SLOT(removePreset()));
    connect(ui->FinishEdit,         SIGNAL(clicked()), this, SLOT(finishEdit()));
    connect(ui->ClassCombo,         SIGNAL(currentIndexChanged(QString)), this, SLOT(updateLibTable(QString)));
    connect(ui->TreeWidget1_5W,     SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(validate(QTreeWidgetItem*, int)));
    connect(ui->TreeWidget3_5W,     SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(validate(QTreeWidgetItem*, int)));
    connect(ui->TreeWidgetCO2,      SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(validate(QTreeWidgetItem*, int)));
    connect(ui->TreeWidgetDS,       SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(validate(QTreeWidgetItem*, int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setArguments(QString defaultPath, QString locale, QString laserType)
{
    mLocale = locale;
    mUsingLaserType = laserType;

    tableInitialize();

    if(defaultPath != "") {
        ui->LibraryPath->setText(defaultPath);
        loadLibrary();
        ui->frame_2->hide();
    }
}

QString MainWindow::getLocaleStr(int lang)
{
    return QVariant::fromValue((EN_SUPPORTED_LANG)lang).value<QString>().toLower();
}

void MainWindow::findLibrary()
{
    QString file = QFileDialog::getOpenFileName(NULL, tr("Open Preset File"), ".", "library (*.json *.fdsp)");
    ui->LibraryPath->setText(file);
}

void MainWindow::loadLibrary()
{
    mTagList.clear();
    ui->ClassCombo->clear();
    ui->ClassCombo->addItem("All");
    mData.clear();

    int ret = loadLibData();
    if(ret != SUCCESS) {
        int ret = QMessageBox::question(this, tr("info"), tr("Cannot open Library.\nCreate new Librarys?"), (QMessageBox::Yes | QMessageBox::No) );
        if(ret == QMessageBox::No) {
            return ;
        } else {
            QString libPath = QFileDialog::getSaveFileName(NULL, tr("Open Image Files"), ".", "library (*.json *.fdsp)");
            ui->LibraryPath->setText(libPath);
        }
    }

    createLibTable();
}

int MainWindow::loadLibData()
{
    QString path = ui->LibraryPath->text();
    QFile file(path);

    if(file.open(QIODevice::ReadWrite) == false) {
        return ERR_PROJ_LOAD_FILE_OPEN;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject json = doc.object();

    QJsonArray matData = json[JSON_TAG_MATERIAL_DATA].toArray();

    foreach (QJsonValue matVal, matData) {
        QJsonObject matObj = matVal.toObject();
        if(matObj.isEmpty() == true) { continue; }
        QString laserType = matObj[JSON_TAG_LASER_TYPE].toString();
        QJsonArray array = matObj[JSON_TAG_PROCESS_PARAM].toArray();
        setData(laserType, array);
    }

    return SUCCESS;
}

void MainWindow::removePreset()
{
    QString currentLaser = currentLaserType();
    CustomTreeWidget* targetTree = currentTree();

    MaterialListedData* listedData;
    QTreeWidgetItem* item = targetTree->currentItem();
    if(item == NULL || item->parent() == NULL) {
        return;
    }
    while(item->parent()->parent() != NULL) {
        item = item->parent();
    }

    foreach (MaterialListedData* data, mData.values(currentLaser)) {
        if(data->buddy() == item) {
            QTreeWidgetItem* parent = item->parent();
            parent->removeChild(item);
            if(parent->childCount() == 0) {
                int topindex = targetTree->indexOfTopLevelItem(parent);
                targetTree->takeTopLevelItem(topindex);
            }
            listedData = data;
            break;
        }
    }

    if(item != NULL) {
        delete item;
    }

    mData.remove(currentLaser, listedData);

}

void MainWindow::addNewPreset()
{
    AddDataDialog addDialog;
    int ret = addDialog.exec();
    if(ret == QDialog::Rejected) { return; }

    QMap<QString, QString> nameList = addDialog.getName();
    QMap<QString, QString> clasList = addDialog.getClass();

    double thickness = addDialog.getThickness();

    QString currentClas = clasList[getLocaleStr(0)];

    if(mTagList.contains(currentClas) == false) {
        mTagList.append(currentClas);
        ui->ClassCombo->addItem(currentClas);
    }
    ui->ClassCombo->setCurrentText(currentClas);

    QString comment = addDialog.getCommnet();

    MaterialListedData* listedData;
    QString currentLaser = currentLaserType();
    if(currentLaser == JSON_TAG_LASER_TYPE_1_6W || mUsingLaserType == "") {
        listedData = new MaterialListedData(nameList, clasList, thickness, comment);
        mData.insert(JSON_TAG_LASER_TYPE_1_6W, listedData);
        insertTableRow(JSON_TAG_LASER_TYPE_1_6W, listedData);
    } else if(currentLaser == JSON_TAG_LASER_TYPE_3_5W || mUsingLaserType == "") {
        listedData = new MaterialListedData(nameList, clasList, thickness, comment);
        mData.insert(JSON_TAG_LASER_TYPE_3_5W, listedData);
        insertTableRow(JSON_TAG_LASER_TYPE_3_5W, listedData);
    } else if(currentLaser == JSON_TAG_LASER_TYPE_CO2 || mUsingLaserType == "") {
        listedData = new MaterialListedData(nameList, clasList, thickness, comment);
        mData.insert(JSON_TAG_LASER_TYPE_CO2, listedData);
        insertTableRow(JSON_TAG_LASER_TYPE_CO2, listedData);
    } else if(currentLaser == JSON_TAG_LASER_TYPE_DS || mUsingLaserType == "") {
        listedData = new MaterialListedData(nameList, clasList, thickness, comment);
        mData.insert(JSON_TAG_LASER_TYPE_DS, listedData);
        insertTableRow(JSON_TAG_LASER_TYPE_DS, listedData);
    }
}

void MainWindow::finishEdit()
{
    QString path = ui->LibraryPath->text();

    QFileInfo fInfo(path);
    if(fInfo.exists() == false) {
        path = QFileDialog::getSaveFileName(NULL, tr("Open Image Files"), ".",
                                            "library (*.json *.fdsp)");
        ui->LibraryPath->setText(path);
    }

    QFile file(path);
    if(file.open(QIODevice::WriteOnly) == false) {
        WARNING_STRING("ERR_PROJ_SAVE_FILE_OPEN_STR");
        return ;
    }

    applyToData();

    QJsonObject body;
    body[JSON_TAG_FILE_TYPE] = FILE_TYPE_MATERIAL;
    writeDown(body);

    QJsonDocument doc(body);
    file.write(doc.toJson());

    file.close();
    close();
}

void MainWindow::setData(const QString laserType, const QJsonArray& json)
{
    foreach (QJsonValue matVal, json) {
        QJsonObject imgObj = matVal.toObject();

        QMap<QString, QString> nameList;
        QMap<QString, QString> clasList;

        QString currentClas = "";

        for(int i = MainWindow::EN; i < MainWindow::LANG_CENTINEL; i++) {
            QString localeStr = getLocaleStr((EN_SUPPORTED_LANG)i);
            if(localeStr == "") { continue; }

            QString name = imgObj[JSON_TAG_MATERIAL_NAME + "_" + localeStr].toString();
            QString clas = imgObj[JSON_TAG_ATTRIBUTION + "_" + localeStr].toString();
            nameList.insert(localeStr, name);
            clasList.insert(localeStr, clas);
            if(mLocale == localeStr) { currentClas = clas; }
        }
        if(currentClas == "") {
            currentClas = clasList.value(getLocaleStr(MainWindow::EN));
        }

        qreal thick = imgObj[JSON_TAG_THICK].toDouble();
        QString comment = imgObj[JSON_TAG_COMMENT].toString("");
        MaterialListedData* listedData = new MaterialListedData(nameList, clasList, thick, comment);
        listedData->setData(imgObj);
        mData.insert(laserType, listedData);
        if(mTagList.contains(currentClas) == false) {
            mTagList.append(currentClas);
        }
    }
}

void MainWindow::writeDown(QJsonObject &json)
{
    QJsonArray dataArray;
    foreach (QString laserType, mData.uniqueKeys()) {
        QJsonObject laserObject;
        laserObject[JSON_TAG_LASER_TYPE] = laserType;
        QJsonArray paramArray;
        foreach (MaterialListedData* data, mData.values(laserType)) {
            QJsonObject paramObj;
            paramObj[JSON_TAG_THICK] = data->thick();
            paramObj[JSON_TAG_COMMENT] = data->comment();

            for(int i = MainWindow::EN; i < MainWindow::LANG_CENTINEL; i++) {
                QString localeStr = getLocaleStr((EN_SUPPORTED_LANG)i);
                if(localeStr == "") { continue; }

                paramObj[JSON_TAG_MATERIAL_NAME + "_" + localeStr] = data->name(localeStr);
                paramObj[JSON_TAG_ATTRIBUTION + "_" + localeStr] = data->tag(localeStr);
            }

            data->writeDown(paramObj);
            paramArray.append(paramObj);
        }
        laserObject[JSON_TAG_PROCESS_PARAM] = paramArray;
        dataArray.append(laserObject);
    }
    json[JSON_TAG_MATERIAL_DATA] = dataArray;
}

void MainWindow::applyToData()
{
    foreach (MaterialListedData* data, mData.values()) {
        data->update();
    }
}

void MainWindow::createLibTable()
{
    ui->ClassCombo->addItems(mTagList);

    createTrees();

    updateLibTable(ui->ClassCombo->currentText());
}

void MainWindow::createTrees()
{
    foreach (QString laserType, mData.uniqueKeys()) {
        foreach (MaterialListedData* data, mData.values(laserType)) {
            insertTableRow(laserType, data);
        }
    }
}

void MainWindow::updateLibTable(QString currentTag)
{
    foreach (QString laserType, mData.uniqueKeys()) {
        foreach (MaterialListedData* data, mData.values(laserType)) {
            data->showTag(mLocale, currentTag);
        }
    }
}

void MainWindow::validate(QTreeWidgetItem* item, int col)
{
    bool isOk = false;
    int value = item->text(col).toInt(&isOk);
    if(isOk == false) {
        value = -1;
    }
    bool changed = false;
    switch(col) {
    case 0: { //type
            QTreeWidgetItem* parent = item->parent();
            if(parent->indexOfChild(item) == 0) {
                item->setText(col, JSON_TAG_USE_TYPE_ENGRAVE);
            } else {
                item->setText(col, JSON_TAG_USE_TYPE_CUT);
            }
            return;
        }
    case 1://speed
        if(value < PATH_PARAMETER_SPEED_MIN) {
            changed = true;
            value = PATH_PARAMETER_SPEED_MIN;
        } else if(value > PATH_PARAMETER_SPEED_MAX) {
            changed = true;
            value = PATH_PARAMETER_SPEED_MAX;
        }
        break;
    case 2://laser pow
        if(value < PATH_PARAMETER_LASER_MIN) {
            changed = true;
            value = PATH_PARAMETER_LASER_MIN;
        } else if(value > PATH_PARAMETER_LASER_MAX) {
            changed = true;
            value = PATH_PARAMETER_LASER_MAX;
        }
        break;
    case 3://process times
        if(value < PATH_PARAMETER_PROCESS_MIN) {
            changed = true;
            value = PATH_PARAMETER_PROCESS_MIN;
        } else if(value > PATH_PARAMETER_PROCESS_MAX) {
            changed = true;
            value = PATH_PARAMETER_PROCESS_MAX;
        }
        break;
    }
    if(changed == true) {
        item->setText(col, QString::number(value));
    }
}

void MainWindow::insertTableRow(QString laserType, MaterialListedData* data)
{
    QTabBar* tabbar = ui->LaserTypeTabs->tabBar();
    int index = -1;
    for(int i = 0; i < tabbar->count(); i++) {
        QString tabText = tabbar->tabText(i);
        if(tabText == laserType) {
            index = i;
            break;
        }
    }

    if(index == -1)
    { return; }

    QWidget* wid = ui->LaserTypeTabs->widget(index);
    QList<CustomTreeWidget*> trees = wid->findChildren<CustomTreeWidget*>();
    if(trees.count() == 0)
    { return ; }

    CustomTreeWidget* tree = trees.at(0);

    MaterialListedData* listedData = (MaterialListedData*)data;

    QTreeWidgetItem* root = NULL;
    QString materialName = listedData->name(mLocale);
    if(materialName == "") {
        materialName = listedData->name(getLocaleStr(MainWindow::EN));
    }
    bool isExist = false;
    for(int i = 0; i < tree->topLevelItemCount(); i++) {
        if(tree->topLevelItem(i)->text(0) == materialName) {
            isExist = true;
            root = tree->topLevelItem(i);
            break;
        }
    }
    if(isExist == false) {
        root  = new QTreeWidgetItem();
        root->setText(0, materialName);
    }

    QTreeWidgetItem* thickRoot = new QTreeWidgetItem();
    thickRoot->setText(0, QString::number(listedData->thick()));
    listedData->setBuddy(thickRoot);

    ParamData param = listedData->param(PARAM_USE_TYPE_ENGRAVE);
    QTreeWidgetItem* child = new QTreeWidgetItem();
    child->setFlags(Qt::ItemIsEditable | child->flags());
    child->setText(0, JSON_TAG_USE_TYPE_ENGRAVE);
    child->setText(1, QString::number(param.headSpeed()));
    child->setText(2, QString::number(param.laserPower()));
    child->setText(3, QString::number(param.processTimes()));
    child->setText(4, listedData->comment());
    thickRoot->addChild(child);

    param = listedData->param(PARAM_USE_TYPE_CUT);
    child = new QTreeWidgetItem();
    child->setFlags(Qt::ItemIsEditable | child->flags());
    child->setText(0, JSON_TAG_USE_TYPE_CUT);
    child->setText(1, QString::number(param.headSpeed()));
    child->setText(2, QString::number(param.laserPower()));
    child->setText(3, QString::number(param.processTimes()));
    thickRoot->addChild(child);

    root->addChild(thickRoot);

    tree->insertTopLevelItem(tree->topLevelItemCount(), root);
}

void MainWindow::tableInitialize()
{
    QList<CustomTreeWidget* > widList = ui->LaserTypeTabs->findChildren<CustomTreeWidget* >();
    foreach (CustomTreeWidget* tree, widList) {
        tree->clear();
        QStringList header;
        header << "Thick" << "Speed" << "LaserPower" << "ProcessTimes" << "Comment";
        tree->setColumnCount(5);
        tree->setHeaderLabels(header);
    }

    if(mUsingLaserType != "") {
        if(mUsingLaserType.indexOf(JSON_TAG_LASER_TYPE_1_6W) != -1) {
            ui->LaserTypeTabs->setCurrentIndex(0);
            ui->TreeWidget1_5W->show();
            ui->TreeWidget3_5W->hide();
            ui->TreeWidgetCO2->hide();
            ui->TreeWidgetDS->hide();
        } else if(mUsingLaserType.indexOf(JSON_TAG_LASER_TYPE_3_5W) != -1) {
            ui->LaserTypeTabs->setCurrentIndex(1);
            ui->TreeWidget1_5W->hide();
            ui->TreeWidget3_5W->show();
            ui->TreeWidgetCO2->hide();
            ui->TreeWidgetDS->hide();
        } else if(mUsingLaserType.indexOf(JSON_TAG_LASER_TYPE_CO2) != -1) {
            ui->LaserTypeTabs->setCurrentIndex(2);
            ui->TreeWidget1_5W->hide();
            ui->TreeWidget3_5W->hide();
            ui->TreeWidgetCO2->show();
            ui->TreeWidgetDS->hide();
        } else if(mUsingLaserType.indexOf(JSON_TAG_LASER_TYPE_DS) != -1) {
            ui->LaserTypeTabs->setCurrentIndex(3);
            ui->TreeWidget1_5W->hide();
            ui->TreeWidget3_5W->hide();
            ui->TreeWidgetCO2->hide();
            ui->TreeWidgetDS->show();
        }
        ui->LaserTypeTabs->tabBar()->hide();
    }
}

QString MainWindow::currentLaserType()
{
    return ui->LaserTypeTabs->tabText(ui->LaserTypeTabs->currentIndex());
}

CustomTreeWidget* MainWindow::currentTree()
{
    QString currentLaser = currentLaserType();
    CustomTreeWidget* targetWidget;

    if(currentLaser == JSON_TAG_LASER_TYPE_1_6W) {
        targetWidget = ui->TreeWidget1_5W;
    } else if(currentLaser == JSON_TAG_LASER_TYPE_3_5W) {
        targetWidget = ui->TreeWidget3_5W;
    } else if(currentLaser == JSON_TAG_LASER_TYPE_CO2) {
        targetWidget = ui->TreeWidgetCO2;
    } else if(currentLaser == JSON_TAG_LASER_TYPE_DS) {
        targetWidget = ui->TreeWidgetDS;
    }

    return targetWidget;
}

MaterialListedData* MainWindow::dataWithDefaults(QString name, QString clas, qreal thick)
{
    foreach(MaterialListedData* data, mData.values(currentLaserType())) {
        if(data->name(getLocaleStr(MainWindow::EN)) == name
                && data->tag(getLocaleStr(MainWindow::EN)) == clas
                && data->thick() == thick) {
            return data;
        }
    }
    return NULL;
}

