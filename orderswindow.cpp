#include "orderswindow.h"
#include "ui_orderswindow.h"

OrdersWindow::OrdersWindow(User *u, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OrdersWindow)
{
    ui->setupUi(this);
    qModel = new QSqlQueryModel;
    ui->OrdersTableView->setModel(qModel);
    us = u;
}

OrdersWindow::~OrdersWindow()
{
    delete qModel;
    delete ui;
    delete this;
}

void OrdersWindow::InitOrders()
{
    QSqlQuery query;

    query.prepare("select u.login as 'Login',  u.name as 'Imię', u.surname as 'Nazwisko', i.name as 'Przedmiot', i.price as 'Cena', u.adress as 'Adres' "
                  "from orders o, users u, items i "
                  "where o.id_user = u.id and o.id_item = i.id");
    query.exec();

    qModel->setQuery(query);
}

void OrdersWindow::InitBought()
{
    QSqlQuery query;

    query.prepare("select i.name as 'Przedmiot', i.price as 'Cena' "
                  "from orders o, users u, items i "
                  "where o.id_user = u.id and o.id_item = i.id and u.login = '" + us->getLogin() + "'");
    query.exec();

    qModel->setQuery(query);
}
