#ifndef ADDDATADIALOG_H
#define ADDDATADIALOG_H

#include <QDialog>

namespace Ui
{
class AddDataDialog;
}

class AddDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDataDialog(QWidget *parent = 0);
    ~AddDataDialog();

    double getThickness();
    QMap<QString, QString> getClass();
    QMap<QString, QString> getName();
    QString getCommnet();


private slots:
    void checkOK();


private:
    Ui::AddDataDialog *ui;
};

#endif // ADDDATADIALOG_H
