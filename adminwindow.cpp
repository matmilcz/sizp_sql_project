#include "adminwindow.h"
#include "ui_adminwindow.h"

adminwindow::adminwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::adminwindow)
{
    ui->setupUi(this);
}

adminwindow::~adminwindow()
{
    delete ui;
}

void adminwindow::on_EditUsersPushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->BackPushButton->setEnabled(true);
}

void adminwindow::on_BackPushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->BackPushButton->setEnabled(false);
}

void adminwindow::on_EditItemsPushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->BackPushButton->setEnabled(true);
}

void adminwindow::on_DeleteUserPushButton_clicked()
{
    if(db.open())
    {
        QSqlQuery query;

        QString login_str = ui->ChooseUserLineEdit->text(); // login użytkownika do usunięcia

        query.prepare("delete from users where login = '" + login_str + "'");

        if(!query.exec())   // usuń użytkownika
            SQLError();    // błąd wykonania zapytania
    }
    else SQLError();    // błąd otwarcia bazy danych
}
