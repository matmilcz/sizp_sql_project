#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include <QtSql>
#include <QListWidgetItem>

#include "adminwindow.h"

extern QSqlDatabase db;

namespace Ui {
class MainWindow;
}

class User
{
    QString login;
    QString email;
    QString name;
    QString surname;
    QString adress;
    int role; // 0 - niezarejestrowany, 1 - zarejestrowany, 2 - admin

public:
    User(QString l = "Niezarejestrowany", QString e = "", QString n = "ADMIN", QString s = "", QString a = "", int r = 0)
        : login(l), email(e), name(n), surname(s), adress(a), role(r) {}
    void ChangeUser(QString l, QString e, QString n, QString s, QString a, int r)
    {
        login = l;
        email = e;
        name = n;
        surname = s;
        adress = a;
        role = r;
    }
    QString getLogin() { return login; }
    QString getEmail() { return email; }
    QString getName() { return name; }
    QString getSurname() { return surname; }
    QString getAdress() { return adress; }
    int getRole() { return role; }

};

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

    void on_CategoryListWidget_itemClicked(QListWidgetItem *item);

    void on_LogOutPushButton_clicked();

    void on_BackPushButton_clicked();

    void on_SubcategoryListWidget_itemClicked(QListWidgetItem *item);

    void on_ItemsListWidget_itemClicked(QListWidgetItem *item);

    void on_AdminPanelPushButton_clicked();

private:
    Ui::MainWindow *ui;

    // My items
    User *us;

    // My Functions
    void FillListWidget(QSqlQuery, QListWidget *);
    int LoginAttempt();  // 1-zalogowano, 0-nie zalogowano
    void CreateNewAccount();
    void SQLError();
    void InitUserPanel();

};

#endif // MAINWINDOW_H
