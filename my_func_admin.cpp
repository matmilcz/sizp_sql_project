#ifndef MY_FUNC_ADMIN_H
#define MY_FUNC_ADMIN_H

#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include <QtSql>
#include <QListWidget>

#include "adminwindow.h"

void adminwindow::FillListWidget(QSqlQuery query, QListWidget *w)
{
    if(query.exec())
    {
        w->clear();
        while(query.next())
        {
            QString name_str = query.value(query.record().indexOf("name")).toString();
            //int price_str = query.value(query.record().indexOf("price")).toInt();
            //QString str = name_str + "      Cena: " + QString::number(price_str);
            w->addItem(name_str);
        }
    }
    else SQLError();    // błąd wykonania zapytania
}

int adminwindow::AddModel(QString model_str, QString producent_str, QString category_str, QString subcategory_str)
{
    int id_info, id1, id2;

    QSqlQuery query;

    query.prepare("select cat.id, subcat.id from categories cat, subcategories subcat "
                  "where subcat.id_category = cat.id "
                  "and cat.name = '" + category_str + "' and subcat.name = '" + subcategory_str + "'");

    if(query.exec())
    {
        query.first();

        id1 = query.value(0).toInt();   // id_category
        id2 = query.value(1).toInt();   // id_subcategory
    }
    else SQLError();   // znajdź id kategorii i podkategorii

    query.prepare("insert into info values('" + model_str + "', '" + producent_str + "', :id1, :id2)");
    query.bindValue(":id1", id1);    // id_category
    query.bindValue(":id2", id2);    // id_subcategory

    if(!query.exec()) SQLError();   // wstaw do tabeli rekord z nowym modelem

    query.prepare("select inf.id from info inf "
                  "where inf.model = '" + model_str + "' and inf.producent = '" + producent_str + "'");

    if(query.exec())
    {
        query.first();
        id_info = query.value(0).toInt();   // zapisz id nowego modelu
    }
    else SQLError();


    return id_info;
}

int adminwindow::AddItem(QString item_str, int price, int id_info)
{
    int id_item;

    QSqlQuery query;

    query.prepare("insert into items values('" + item_str + "', :price, :id_info)");
    query.bindValue(":price", price);
    query.bindValue(":id_info", id_info);

    if(!query.exec()) SQLError(); // dodaj przedmiot

    query.prepare("select id from items "
                  "where name = '" + item_str + "'");

    if(query.exec())
    {
        query.first();
        id_item = query.value(0).toInt();   // zapisz id nowego przedmiotu
    }
    else SQLError();

    return id_item;
}

void adminwindow::ChangeQuantity(int id_item, int quantity)
{
    QSqlQuery query;

    query.prepare("select quantity from warehouse where id_item = :id");
    query.bindValue(":id", id_item);

    if(query.exec())    // przejrzyj rekordy w poszukiwaniu istniejącego modelu
    {
        query.first();

        if(query.isNull(0))
        {
            query.prepare("insert into warehouse values(:id, :q)");
            query.bindValue(":id", id_item);
            query.bindValue(":q", quantity);

            if(!query.exec()) SQLError();
        }
        else
        {
            query.prepare("update warehouse set quantity = :q where id_item = :id");
            query.bindValue(":q", quantity);
            query.bindValue(":id", id_item);

            if(!query.exec()) SQLError();
        }
    }
    else SQLError();    // błąd wykonania zapytania
}


#endif // MY_FUNC_ADMIN_H
