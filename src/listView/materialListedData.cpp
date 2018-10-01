#include "materialListedData.h"
#include <QJsonObject>

MaterialListedData::MaterialListedData(const QMap<QString, QString>& nameList, const QMap<QString, QString>& classList, qreal thicks, QString comment):
    ListedDataCore(nameList, classList)
{
    mThick = thicks;
    mCut.setType(PARAM_USE_TYPE_CUT);
    mBuddy = NULL;
    mComment = comment;
}

MaterialListedData::MaterialListedData():
    ListedDataCore()
{
    mThick = 0;
    mBuddy = NULL;
    mComment = "";
}

void MaterialListedData::setData(const QJsonObject& json)
{
    QJsonObject paramObj = json[JSON_TAG_PARAMETER].toObject();

    QJsonObject engrave  = paramObj[JSON_TAG_USE_TYPE_ENGRAVE].toObject();
    mEngrave.setHeadSpeed(engrave[JSON_TAG_PATH_SPEED].toInt());
    mEngrave.setLaserPower(engrave[JSON_TAG_PATH_LASER].toInt());
    mEngrave.setProcessTimes(engrave[JSON_TAG_PATH_TIMES].toInt());

    QJsonObject cut = paramObj[JSON_TAG_USE_TYPE_CUT].toObject();
    mCut.setHeadSpeed(cut[JSON_TAG_PATH_SPEED].toInt());
    mCut.setLaserPower(cut[JSON_TAG_PATH_LASER].toInt());
    mCut.setProcessTimes(cut[JSON_TAG_PATH_TIMES].toInt());
}

void MaterialListedData::writeDown(QJsonObject& json)
{
    QJsonObject paramObj;
    QJsonObject engrave;
    engrave[JSON_TAG_PATH_SPEED] = mEngrave.headSpeed();
    engrave[JSON_TAG_PATH_LASER] = mEngrave.laserPower();
    engrave[JSON_TAG_PATH_TIMES] = mEngrave.processTimes();
    paramObj[JSON_TAG_USE_TYPE_ENGRAVE] = engrave;

    QJsonObject cut;
    cut[JSON_TAG_PATH_SPEED] = mCut.headSpeed();
    cut[JSON_TAG_PATH_LASER] = mCut.laserPower();
    cut[JSON_TAG_PATH_TIMES] = mCut.processTimes();
    paramObj[JSON_TAG_USE_TYPE_CUT] = cut;

    json[JSON_TAG_PARAMETER] = paramObj;
}


ParamData MaterialListedData::param(PARAM_USE_TYPE type)
{
    if(type == PARAM_USE_TYPE_ENGRAVE) {
        return mEngrave;
    } else {
        return mCut;
    }
}

void MaterialListedData::update()
{
    if(mBuddy == NULL) { return; }

    QTreeWidgetItem* engrave = mBuddy->child(0);
    QTreeWidgetItem* cut     = mBuddy->child(1);

    mEngrave.setHeadSpeed(engrave->text(1).toInt());
    mEngrave.setLaserPower(engrave->text(2).toInt());
    mEngrave.setProcessTimes(engrave->text(3).toInt());
    mCut.setHeadSpeed(cut->text(1).toInt());
    mCut.setLaserPower(cut->text(2).toInt());
    mCut.setProcessTimes(cut->text(3).toInt());

    mComment = QString(engrave->text(4).toLocal8Bit());
}

void MaterialListedData::showTag(QString locale, QString tag)
{
    if(mBuddy == NULL) { return; }

    if(locale == "") {
        locale = "EN";
    }
    QString currentTag = this->tag(locale);

    if(tag == "All") {
        mBuddy->setHidden(false);
    } else if(currentTag == tag) {
        mBuddy->setHidden(false);
    } else {
        mBuddy->setHidden(true);
    }
}
