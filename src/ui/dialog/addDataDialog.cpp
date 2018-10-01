#include "addDataDialog.h"
#include "ui_addDataDialog.h"
#include "MainWindow.h"

AddDataDialog::AddDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDataDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(checkOK()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QTableWidget* nameTable = ui->FormalNameTable;
    QStringList verticalHeader, horizontalHeader;

    horizontalHeader << "Formal Name" << "Classification";


    for(int i = MainWindow::EN; i < MainWindow::LANG_CENTINEL; i++) {
        QString localeStr = MainWindow::getLocaleStr(i);
        if(localeStr == "") { continue; }

        verticalHeader << localeStr;
    }

    nameTable->setColumnCount(horizontalHeader.count());
    nameTable->setHorizontalHeaderLabels(horizontalHeader);
    nameTable->setRowCount(verticalHeader.count());
    nameTable->setVerticalHeaderLabels(verticalHeader);
}

AddDataDialog::~AddDataDialog()
{
    delete ui;
}

void AddDataDialog::checkOK()
{
    QTableWidget* table = ui->FormalNameTable;
    if(table->item(0, 1) == 0 || table->item(0, 0) == 0) { return; }
    QString defClass = table->item(0, 1)->text();
    QString defName  = table->item(0, 0)->text();
    if(defClass.isEmpty() || defName.isEmpty()) {
        return;
    }
    accept();
}

double AddDataDialog::getThickness()
{
    return ui->ThicknessEdit->value();
}

QMap<QString, QString> AddDataDialog::getClass()
{
    QMap<QString, QString> ret;
    QTableWidget* table = ui->FormalNameTable;
    for(int i = MainWindow::EN; i < MainWindow::LANG_CENTINEL; i++) {
        QString localeStr = MainWindow::getLocaleStr(i);
        if(localeStr == "" || table->item(i, 1) == 0) { continue; }
        ret.insert(localeStr, table->item(i, 1)->text());
    }
    return ret;
}

QMap<QString, QString> AddDataDialog::getName()
{
    QMap<QString, QString> ret;
    QTableWidget* table = ui->FormalNameTable;
    for(int i = MainWindow::EN; i < MainWindow::LANG_CENTINEL; i++) {
        QString localeStr = MainWindow::getLocaleStr(i);
        if(localeStr == "" || table->item(i, 0) == 0) { continue; }
        ret.insert(localeStr, table->item(i, 0)->text());
    }
    return ret;
}

QString AddDataDialog::getCommnet()
{
    return ui->CommentEdit->text();
}
