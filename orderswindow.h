#ifndef ORDERSWINDOW_H
#define ORDERSWINDOW_H

#include <QDialog>
#include <QtSql>

#include "user.h"

namespace Ui {
class OrdersWindow;
}

class OrdersWindow : public QDialog
{
    Q_OBJECT

public:
    explicit OrdersWindow(User *u = NULL, QWidget *parent = 0);
    ~OrdersWindow();

private:
    Ui::OrdersWindow *ui;

    // My Stuff

    QSqlQueryModel *qModel;
    User *us;

public:
    void InitBought();
    void InitOrders();
};

#endif // ORDERSWINDOW_H
