#ifndef MATERIALLISTEDDATA_H
#define MATERIALLISTEDDATA_H

#include <QObject>
#include <QTreeWidget>

#include "listedDataCore.h"
#include "common/paramData.h"

class MaterialListedData : public ListedDataCore
{
public:
    MaterialListedData(const QMap<QString, QString>& nameList, const QMap<QString, QString>& classList, qreal thicks, QString comment = "");
    MaterialListedData();
    ~MaterialListedData();

    qreal thick() { return mThick; }
    ParamData param(PARAM_USE_TYPE type);
    QString comment() { return mComment; }
    void setComment(QString comment) { mComment = comment; }

    void setBuddy(QTreeWidgetItem* buddy) { mBuddy = buddy; }
    QTreeWidgetItem* buddy() { return mBuddy; }
    void update();
    void showTag(QString locale, QString tag);

    virtual void setData(const QJsonObject& json) override;
    void writeDown(QJsonObject& json);
    enum { Type = LIST_DATA_MATERIAL };
    int type() const
    {
        return Type;
    }

private:
    qreal       mThick;
    ParamData   mEngrave;
    ParamData   mCut;
    QTreeWidgetItem* mBuddy;    //Root
    QString     mComment;
};
Q_DECLARE_METATYPE(MaterialListedData*)
#endif // MATERIALLISTEDDATA_H
