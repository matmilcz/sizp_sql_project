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
    delete this;
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

    // zainicjalizuj combobox i listę przedmiotów
    if(db.open())
    {
        QSqlQuery query;

        // zainicjalizuj combobox

        if(query.exec("select c.name from categories c"))
        {
            ui->CategoryComboBox->clear();
            while(query.next()) ui->CategoryComboBox->addItem(query.value(0).toString());
        }
        else SQLError();    // błąd wykonania zapytania


        // zainicjalizuj listę przedmiotów

        query.prepare("select * from items order by name");
        FillListWidget(query, ui->ChooseItemsListWidget);

        db.close();
    }
    else SQLError();    // błąd otwarcia bazy danych


    ui->CategoryComboBox->setCurrentIndex(1);
    ui->CategoryComboBox->blockSignals(false);

    ui->CategoryComboBox->setCurrentIndex(0);
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

        db.close();
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

        db.close();
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

        db.close();
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

        // odśwież listę

        query.prepare("select * from items order by name");
        FillListWidget(query, ui->ChooseItemsListWidget);

        db.close();
    }
    else SQLError();
}

void adminwindow::on_DeleteItemPushButton_clicked()
{
    if(db.open())
    {
        QSqlQuery query;

        query.prepare("delete from items where name = '" + ui->ChooseItemsListWidget->currentItem()->text() + "'");

        if(query.exec())
        {
            SQLError("Pomyślnie usunięto rekord");

            // zainicjalizuj nową listę przedmiotów

            query.prepare("select * from items order by name");
            FillListWidget(query, ui->ChooseItemsListWidget);
        }
        else SQLError();

        db.close();
    }
    else SQLError();
}

void adminwindow::on_ChooseItemsListWidget_itemClicked(QListWidgetItem *item)
{
    if(db.open())
    {
        QSqlQuery query;

        QString name_str = item->text();

        //SQLError(name_str);

        query.prepare("select it.name, inf.producent, inf.model, it.price, war.quantity, cat.name, subcat.name "
                      "from warehouse war, items it, info inf, categories cat, subcategories subcat "
                      "where war.id_item = it.id and it.id_info = inf.id and inf.id_category = cat.id and inf.id_subcategory = subcat.id "
                      "and it.name = '" + name_str + "'");
        //query.bindValue(":str", name_str);


        if(query.exec())
        {
            query.first();

            ui->NameLineEdit->setText(query.value(0).toString());
            ui->ProducentLineEdit->setText(query.value(1).toString());
            ui->ModelLineEdit->setText(query.value(2).toString());

            ui->PriceSpinBox->setValue(query.value(3).toInt());
            ui->QuantitySpinBox->setValue(query.value(4).toInt());

            ui->CategoryComboBox->blockSignals(true);
            ui->CategoryComboBox->setCurrentText(query.value(5).toString());
            ui->CategoryComboBox->blockSignals(false);

            QString sub_str = query.value(6).toString();

            query.prepare("select s.name from subcategories s, categories c "
                          "where c.id = s.id_category and c.name = '" + ui->CategoryComboBox->currentText() + "'");

            if(query.exec())
            {
                ui->SubcategoryComboBox->clear();
                while(query.next()) ui->SubcategoryComboBox->addItem(query.value(0).toString());
            }
            else SQLError();    // błąd wykonania zapytania

            ui->SubcategoryComboBox->setCurrentText(sub_str);
        }
        else SQLError();

        db.close();
    }
    else SQLError();
}

void adminwindow::on_EditItemPushButton_clicked()
{
    QString item_str = ui->NameLineEdit->text(),
            model_str = ui->ModelLineEdit->text(),
            producent_str = ui->ProducentLineEdit->text(),
            category_str = ui->CategoryComboBox->currentText(),
            subcategory_str = ui->SubcategoryComboBox->currentText();
    int price = ui->PriceSpinBox->value(),
        quantity = ui->QuantitySpinBox->value();

    QSqlQuery query;

    if(db.open())
    {
        /// Zaktualizuj warehouse

        query.prepare("update warehouse set quantity = :q "
                      "where id_item = (select id from items where name = '" + item_str + "')");
        query.bindValue(":q", quantity);

        if(!query.exec()) SQLError();

        /// Zaktualizuj info
/*
        query.prepare("update info set id_category = (select id from categories where name = '" + category_str + "'), "
                      "id_subcategory = (select id from subcategories where name = '" +subcategory_str  + "') "
                      "where (select id_info from items where name = '" + item_str + "') = id");
*/
        int id_info; // id rekordu
        bool found_model = 0;


        // przejrzyj rekordy w poszukiwaniu istniejącego modelu

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


        // nie znaleziono modelu -> stwórz nową tabelę info

        if(!found_model)
        {
            // dodaj nowy model
            id_info = AddModel(model_str, producent_str, category_str, subcategory_str);

            // zaktualizuj id_info w przedmiocie
            query.prepare("update items set id_info = :id where name = '" + item_str + "'");
            query.bindValue(":id", id_info);
            if(!query.exec()) SQLError();
        }


        // znaleziono model -> sprawdź czy kategorie się zgadzają

        // TODO: to co wyżej napisane plus ew stworzyć nowy model


        /// Zaktualizuj cenę przedmiotu

        query.prepare("update items set price = :p, name = '" + item_str + "' where name = '" + ui->ChooseItemsListWidget->currentItem()->text() + "'");
        query.bindValue(":p", price);

        if(!query.exec()) SQLError();


        /// Odśwież listę

        query.prepare("select * from items order by name");
        FillListWidget(query, ui->ChooseItemsListWidget);

        db.close();
    }
    else SQLError();
}
