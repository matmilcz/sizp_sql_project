#include "adminwindow.h"
#include "ui_adminwindow.h"

#include "my_func_admin.cpp"

adminwindow::adminwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::adminwindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
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
    ui->CategoryComboBox->blockSignals(true);

    // zainicjalizuj combobox
    if(db.open())
    {
        QSqlQuery query;

        //query.prepare("select c.name from categories c");

        if(query.exec("select c.name from categories c"))
        {
            ui->CategoryComboBox->clear();
            while(query.next()) ui->CategoryComboBox->addItem(query.value(0).toString());
        }
        else SQLError();    // błąd wykonania zapytania
    }
    else SQLError();    // błąd otwarcia bazy danych

    ui->CategoryComboBox->blockSignals(false);

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

void adminwindow::on_MakeAdminPushButton_clicked()
{
    if(db.open())
    {
        QSqlQuery query;

        QString login_str = ui->ChooseUserLineEdit->text(); // login użytkownika, któremu podwyższamy uprawnienia

        query.prepare("update users set role = 2 where login = " + login_str +"'");

        if(!query.exec())   // usuń użytkownika
            SQLError();    // błąd wykonania zapytania
    }
    else SQLError();    // błąd otwarcia bazy danych
}

void adminwindow::on_CategoryComboBox_currentIndexChanged(const QString &arg1)
{
    // zainicjalizuj combobox
    if(db.open())
    {
        QSqlQuery query;

        query.prepare("select s.name from subcategories s, categories c "
                      "where c.id = s.id_category and c.name = '" + arg1 + "'");

        if(query.exec())
        {
            ui->SubcategoryComboBox->clear();
            while(query.next()) ui->SubcategoryComboBox->addItem(query.value(0).toString());
        }
        else SQLError();    // błąd wykonania zapytania
    }
    else SQLError();    // błąd otwarcia bazy danych
}

void adminwindow::on_AddItemPushButton_clicked()
{
    QString item_str = ui->NameLineEdit->text(),
            model_str = ui->ModelLineEdit->text(),
            producent_str = ui->ProducentLineEdit->text(),
            category_str = ui->CategoryComboBox->currentText(),
            subcategory_str = ui->SubcategoryComboBox->currentText();
    int price = ui->PriceSpinBox->value(),
        quantity = ui->QuantitySpinBox->value();

    if(db.open())
    {
        QSqlQuery query;

        int id_info, id_item; // id rekordu
        bool found_model = 0;

        /// Przejrzyj rekordy w poszukiwaniu istniejącego modelu

        query.prepare("select inf.model, inf.producent, cat.id, subcat.id, inf.id from info inf, categories cat, subcategories subcat "
                      "where inf.id_category = cat.id and inf.id_subcategory = subcat.id "
                      "and cat.name = '" + category_str + "' and subcat.name = '" + subcategory_str + "'");

        if(query.exec())
        {
            // przejrzyj listę modeli w danej kategorii
            while(query.next())
                // sprawdź czy dany rekord odpowiada szukanemu modelowi
                if(query.value(0).toString() == model_str && query.value(1).toString() == producent_str)
                {
                    found_model = 1;    // ustaw flagę - znaleziono model
                    id_info = query.value(4).toInt();   // zapisz id modelu
                    break;
                }
        }
        else SQLError();    // błąd wykonania zapytania

        /// Nie znaleziono modelu

        if(!found_model)
        {
            // dodaj nowy model
            id_info = AddModel(model_str, producent_str, category_str, subcategory_str);

            // dodaj przedmiot z danego modelu
            id_item = AddItem(item_str, price, id_info);

            // zmień ilość dostępnych przedmiotów
            ChangeQuantity(id_item, quantity);
        }

        /// Znaleziono model

        else
        {
            bool found_item = 0;

            // sprawdź czy istnieje już dany przedmiot

            query.prepare("select id, name from items where name = '" + item_str + "'");

            if(query.exec())
            {
                while(query.next())
                {
                    if(query.value(1).toString() == item_str)
                    {
                        id_item = query.value(0).toInt();
                        found_item = 1; // znaleziono przedmiot - nie trzeba dodawać nowego
                        break;
                    }
                }
            }
            else SQLError();

            if(!found_item) // nie znaleziono przedmiotu
            {
                id_item = AddItem(item_str, price, id_info);
                ChangeQuantity(id_item, quantity);
            }
            else    // znaleziono przedmiot
            {
                query.prepare("update items set price = :p where id_item = :id");
                query.bindValue(":p", price);
                query.bindValue(":id", id_item);

                if(!query.exec()) SQLError();

                ChangeQuantity(id_item, quantity);
            }
        }

        db.close();
    }
    else SQLError();
}
