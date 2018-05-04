#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "my_func_main.cpp"

#define LOGIN_PAGE 0
#define NEW_USER_PAGE 1
#define SHOP_PAGE 2

#define CATEGORY_PAGE 0
#define SUBCATEGORY_PAGE 1
#define ITEMS_PAGE 2
#define ITEM_PAGE 3

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString ServerName = "LOCALHOST\\SQLEXPRESS";
    QString DBName = "zz_sql_project";
    QString dsn = QString("DRIVER={SQL Server};Server=%1;Database=%2;Trusted_Connection=Yes;").arg(ServerName).arg(DBName);

    db = QSqlDatabase::addDatabase("QODBC3");
    db.setConnectOptions();
    db.setDatabaseName(dsn);

    if(!db.open()) SQLError();  // otwórz połączenie z bazą danych

    us = new User();    // nowy użytkownik

    qModelCategory = new QSqlQueryModel;
    ui->CategoryListView->setModel(qModelCategory);
    qModelSubcategory = new QSqlQueryModel;
    ui->SubcategoryListView->setModel(qModelSubcategory);
    qModelItems = new QSqlQueryModel;
    ui->ItemsTreeView->setModel(qModelItems);

    ui->stackedWidget->setCurrentIndex(LOGIN_PAGE); // przełącz na stronę logowania
}

MainWindow::~MainWindow()
{
    if(!db.isOpen()) db.close();

    delete qModelCategory;
    delete qModelSubcategory;
    delete qModelItems;
    delete us;
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

        qModelCategory->setQuery("select name from categories");

        ui->stackedWidget_2->setCurrentIndex(CATEGORY_PAGE);
        ui->BackPushButton->setEnabled(false);
    }
    else SQLError("Błędny login lub hasło");
}

void MainWindow::on_CreateAccountPushButton_clicked()
{
    CreateNewAccount();

    ui->stackedWidget->setCurrentIndex(LOGIN_PAGE); // po założeniu konta - przełącz na stronę logowania
}

void MainWindow::on_LogOutPushButton_clicked()
{
    us->ChangeUser("Niezarejestrowany", "", "", "", "", 0); // wyloguj
    ui->stackedWidget->setCurrentIndex(LOGIN_PAGE); // przejdź do strony logowania
}

void MainWindow::on_BackPushButton_clicked()
{
    if(ui->stackedWidget_2->currentIndex() > CATEGORY_PAGE) ui->stackedWidget_2->setCurrentIndex( ui->stackedWidget_2->currentIndex()-1 );
    if(ui->stackedWidget_2->currentIndex() == CATEGORY_PAGE) ui->BackPushButton->setEnabled(false);
}

void MainWindow::on_CategoryListView_clicked(const QModelIndex &index)
{
    // Wypełnia listę podkategorii

    QSqlRecord record = qModelCategory->record(index.row());
    QString cat_str = record.field(0).value().toString();

    qModelSubcategory->setQuery("select name from subcategories "
                                "where id_category = (select id from categories where name = '" + cat_str + "')");

    ui->stackedWidget_2->setCurrentIndex(SUBCATEGORY_PAGE);
    ui->BackPushButton->setEnabled(true);
}

void MainWindow::on_SubcategoryListView_clicked(const QModelIndex &index)
{
    QSqlRecord record = qModelSubcategory->record(index.row());
    QString subcat_str = record.field(0).value().toString();

    qModelItems->setQuery("select it.name as 'Nazwa', it.price as 'Cena' "
                          "from items it, info inf, categories cat, subcategories subcat "
                          "where it.id_info = inf.id and inf.id_category = cat.id and inf.id_subcategory = subcat.id and "
                          "inf.id_subcategory = (select id from subcategories where name = '" + subcat_str + "')");

    ui->stackedWidget_2->setCurrentIndex(ITEMS_PAGE);
    ui->BackPushButton->setEnabled(true);
}

void MainWindow::on_ItemsTreeView_clicked(const QModelIndex &index)
{
    QSqlRecord record = qModelItems->record(index.row());
    QString item_str = record.field(0).value().toString();

    // Wyświetla przedmiot

    QSqlQuery query;
    query.prepare("select it.name, it.price, inf.model, inf.producent from items it, info inf "
                  "where it.id_info = inf.id and it.id = (select id from items where name = '" + item_str + "')");
                  // zrobić ładniejsze zapytanie
    query.exec();

    query.first();
    QString nameLabel = query.value(0).toString();
    int priceLabel = query.value(1).toInt();
    QString modelLabel = query.value(2).toString();
    QString producentLabel = query.value(3).toString();

    ui->ItemNameLabel->setText(nameLabel);
    ui->ModelLabel->setText(modelLabel);
    ui->ProducentLabel->setText(producentLabel);
    ui->PriceLabel->setText(QString::number(priceLabel) + " PLN");

    ui->stackedWidget_2->setCurrentIndex(ITEM_PAGE);
    ui->BackPushButton->setEnabled(true);
}

void MainWindow::on_AdminPanelPushButton_clicked()
{
    adminwindow *w = new adminwindow;   // wyświetla panel admina
    w->show();
}

void MainWindow::on_SearchLineEdit_editingFinished()
{
    QSqlQuery query;

    if(ui->SearchLineEdit->text() == "")
    {
        ui->stackedWidget_2->setCurrentIndex(CATEGORY_PAGE);
        ui->BackPushButton->setEnabled(false);
    }
    else
    {
        qModelItems->setQuery("select name from items where name like '%" + ui->SearchLineEdit->text() + "%'");

        ui->stackedWidget_2->setCurrentIndex(ITEMS_PAGE);
        ui->BackPushButton->setEnabled(true);
    }
}

void MainWindow::on_BuyPushButton_clicked()
{
    QSqlQuery query;

    query.prepare("insert into orders values( "
                  "(select id from users where login = '" + us->getLogin() + "'), "
                  "(select id from items where name = '" + ui->ItemNameLabel->text() + "'), "
                  "(select adress from users where login = '" + us->getLogin() + "'))");

    if(query.exec())    // dodaj do zamówień
    {
        query.prepare("select w.quantity from warehouse w, items i where i.id = w.id_item and i.name = '" + ui->ItemNameLabel->text() + "'");

        int q;
        if(query.exec())    // znajdź obecną ilość przedmiotu w magazynie
        {
            query.first();
            q = query.value(0).toInt() - 1;
        }
        else SQLError();

        query.prepare("update warehouse set quantity = :q where id_item = (select name from items where name = '" + ui->ItemNameLabel->text() + "')");
        query.bindValue(":q", q);
        if(!query.exec()) SQLError();   // dekrementuj ilość

        SQLError("Kupiono pomyślnie przedmiot");
    }
    else SQLError();
}

void MainWindow::on_OrdersPushButton_clicked()
{
    OrdersWindow *w = new OrdersWindow(us);
    w->show();
    w->InitOrders();
}

void MainWindow::on_BoughtPushButton_clicked()
{
    OrdersWindow *w = new OrdersWindow(us);
    w->show();
    w->InitBought();
}
