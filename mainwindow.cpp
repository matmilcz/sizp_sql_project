#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "my_func.cpp"

#define LOGIN_PAGE 0
#define NEW_USER_PAGE 1
#define SHOP_PAGE 2

#define CATEGORY_PAGE 0
#define SUBCATEGORY_PAGE 1
#define ITEMS_PAGE 2

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    us = new User();

    ui->stackedWidget->setCurrentIndex(LOGIN_PAGE); // przełącz na stronę logowania
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_CreateAccountLinkButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(NEW_USER_PAGE);  // przełącz na stronę zakładania konta
}

void MainWindow::on_LoginPushButton_clicked()
{
    if(LoginAttempt()) // jeśli zalogowano
    {
        ui->stackedWidget->setCurrentIndex(SHOP_PAGE);  // przełącz na stronę sklepu

        InitUserPanel();    // zainicjalizuj panel użytkownika


        if(db.open())
        {
            QSqlQuery query;

            query.prepare("select * from categories");

            FillListWidget(query, ui->CategoryListWidget);
            ui->stackedWidget_2->setCurrentIndex(CATEGORY_PAGE);
            db.close();

            ui->BackPushButton->setEnabled(false);
        }
        else SQLError();
    }
    else
    {
        QMessageBox ErrMsg;
        ErrMsg.setText("Błędny login lub hasło");
        ErrMsg.exec();
    }
}

void MainWindow::on_CreateAccountPushButton_clicked()
{
    // { TODO: Dodawanie użytkownika

    CreateNewAccount();

    // }

    ui->stackedWidget->setCurrentIndex(LOGIN_PAGE); // po założeniu konta - przełącz na stronę logowania
}

void MainWindow::on_CategoryListWidget_itemClicked(QListWidgetItem *item)
{
    // Znajduje id danej kategorii
    int id;

    if(db.open())
    {
        QSqlQuery query;

        query.prepare("select * from categories where name = :name");
        query.bindValue(":name", item->text());

        if(query.exec())
        {
            query.first();
            id = query.value(0).toInt();
        }
        else SQLError();    // błąd wykonania zapytania
    }
    else SQLError();    // błąd otwarcia bazy danych
    //

    // Wypełnia listę
    QSqlQuery query;

    query.prepare("select * from subcategories where id_category = :id");
    query.bindValue(":id", id);

    FillListWidget(query, ui->SubcategoryListWidget);
    ui->stackedWidget_2->setCurrentIndex(SUBCATEGORY_PAGE);
    ui->BackPushButton->setEnabled(true);
    db.close();
}

void MainWindow::on_LogOutPushButton_clicked()
{
    us->ChangeUser("Niezarejestrowany", "", 0); // wyloguj
    ui->stackedWidget->setCurrentIndex(LOGIN_PAGE); // przejdź do strony logowania
}

void MainWindow::on_BackPushButton_clicked()
{
    if(ui->stackedWidget_2->currentIndex() > CATEGORY_PAGE) ui->stackedWidget_2->setCurrentIndex( ui->stackedWidget_2->currentIndex()-1 );
    if(ui->stackedWidget_2->currentIndex() == CATEGORY_PAGE) ui->BackPushButton->setEnabled(false);
}

void MainWindow::on_SubcategoryListWidget_itemClicked(QListWidgetItem *item)
{
    // Znajduje id danej podkategorii
    int id;

    if(db.open())
    {
        QSqlQuery query;

        query.prepare("select * from subcategories where name = :name");
        query.bindValue(":name", item->text());

        if(query.exec())
        {
            query.first();
            id = query.value(0).toInt();
        }
        else SQLError();    // błąd wykonania zapytania
    }
    else SQLError();    // błąd otwarcia bazy danych
    //

    // Wypełnia listę przedmiotami
    QSqlQuery query;

    query.prepare("select * from items i, item_category ic, categories c, subcategories s "
                  "where i.id = ic.id_item and ic.id_category = c.id and ic.id_subcategory = s.id and ic.id_subcategory = :id");
    // zrobić ładniejsze zapytanie
    query.bindValue(":id", id);

    FillListWidget(query, ui->ItemsListWidget);
    ui->stackedWidget_2->setCurrentIndex(ITEMS_PAGE);
    ui->BackPushButton->setEnabled(true);
    db.close();
}
