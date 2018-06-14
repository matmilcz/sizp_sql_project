#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include <QtSql>
#include <QListWidgetItem>

#include "adminwindow.h"
#include "orderswindow.h"
#include "user.h"

extern QSqlDatabase db;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_CreateAccountLinkButton_clicked();

    void on_LoginPushButton_clicked();

    void on_CreateAccountPushButton_clicked();

    void on_LogOutPushButton_clicked();

    void on_BackPushButton_clicked();

    void on_AdminPanelPushButton_clicked();

    void on_SearchLineEdit_editingFinished();

    void on_BuyPushButton_clicked();

    void on_OrdersPushButton_clicked();

    void on_ItemsTreeView_clicked(const QModelIndex &index);

    void on_SubcategoryListView_clicked(const QModelIndex &index);

    void on_CategoryListView_clicked(const QModelIndex &index);

    void on_BoughtPushButton_clicked();

    void on_FreeUserPushButton_clicked();

private:
    Ui::MainWindow *ui;

    // My stuff
    User *us;
    QSqlQueryModel *qModelCategory, *qModelSubcategory, *qModelItems;


    int LoginAttempt();  // 1-zalogowano, 0-nie zalogowano
    void CreateNewAccount();
    void SQLError(QString s = "")
    {
        QMessageBox ErrMsg;
        if(s.isEmpty()) ErrMsg.setText(db.lastError().text());
        else ErrMsg.setText(s);
        ErrMsg.exec();
    }
    void InitUserPanel();

public:
    QSqlDatabase db;

};

#endif // MAINWINDOW_H
