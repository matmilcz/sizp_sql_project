#ifndef MY_FUNC_H
#define MY_FUNC_H

#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include <QtSql>

#include "mainwindow.h"

int MainWindow::LoginAttempt()  // 1-zalogowano, 0-nie zalogowano
{
    QSqlQuery query;

    QString login_str = "";
    QString passwd_str = "";

    if(query.exec("select * from users"))
    {
        while(query.next())
        {
             login_str = query.value(query.record().indexOf("login")).toString(); // szukaj loginu
             passwd_str = query.value(query.record().indexOf("passwd")).toString(); // szukaj hasła

             if(ui->LoginLineEdit->text() == login_str && ui->PasswordLineEdit->text() == passwd_str)
             {
                 // zalogowanie
                 QString email_str = query.value(query.record().indexOf("email")).toString(); // odczytaj email
                 QString name_str = query.value(query.record().indexOf("name")).toString(); // odczytaj email
                 QString surname_str = query.value(query.record().indexOf("surname")).toString(); // odczytaj email
                 QString adress_str = query.value(query.record().indexOf("adress")).toString(); // odczytaj email

                 query.prepare("select r.id from users u, roles r where u.role=r.id and u.login like '" + login_str + "'");
                 if(query.exec()) // znajdź poziom dostępu
                 {
                     query.first();
                     int role = query.value(0).toInt();

                     us->ChangeUser(login_str, email_str, name_str, surname_str, adress_str,role);
                 }
                 else
                 {
                     SQLError();    // błąd wykonania zapytania
                     return 0; // nie zalogowano
                 }
                 //
                 return 1; // zalogowano pomyślnie
             }
        }
    }
    else SQLError();    // błąd wykonania zapytania

    return 0; // nie zalogowano
}

void MainWindow::CreateNewAccount()
{
    QString login_str = ui->CreateLoginLineEdit->text();
    QString passwd_str = ui->CreatePasswordLineEdit->text();
    QString email_str = ui->CreateEmailLineEdit->text();
    QString name_str = ui->CreateNameLineEdit->text();
    QString surname_str = ui->CreateSurnameLineEdit->text();
    QString address_str = ui->CreateAddressLineEdit->text();

    if(!(login_str.isEmpty() || passwd_str.isEmpty() || email_str.isEmpty() || name_str.isEmpty() || surname_str.isEmpty() || address_str.isEmpty()))
    {
        QSqlQuery query;
        query.prepare("insert into users "
                      "values ('" + login_str + "', '" + passwd_str + "', '" + email_str + "', '"
                      + name_str + "', '" + surname_str + "', '" + address_str + "', 1)");

        if(!query.exec()) SQLError();   // dodaj użytkownika
    }
}

void MainWindow::InitUserPanel()
{
    ui->LoginNameLabel->setText(us->getName() + " " + us->getSurname());    // wyświetl nazwę użytkownika

    if(us->getRole() == 2)
    {
        ui->AdminPanelPushButton->setVisible(true);  // pokaż przycisk panelu admina
        ui->OrdersPushButton->setVisible(true);
    }
    else
    {
        ui->AdminPanelPushButton->setVisible(false);   // ukryj przycisk panelu admina
        ui->OrdersPushButton->setVisible(false);
    }

    if(us->getRole() > 0)
    {
        ui->BoughtPushButton->setVisible(true);
    }
    else
    {
        ui->BoughtPushButton->setVisible(false);
    }
}


#endif // MY_FUNC_H
