#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QWidget>
#include <QtSql>
#include <QMessageBox>

extern QSqlDatabase db;

namespace Ui {
class adminwindow;
}

class adminwindow : public QWidget
{
    Q_OBJECT

public:
    explicit adminwindow(QWidget *parent = 0);
    ~adminwindow();

private slots:
    void on_EditUsersPushButton_clicked();

    void on_BackPushButton_clicked();

    void on_EditItemsPushButton_clicked();

    void on_DeleteUserPushButton_clicked();

private:
    Ui::adminwindow *ui;

    void SQLError()
    {
        QMessageBox ErrMsg;
        ErrMsg.setText(db.lastError().text());
        ErrMsg.exec();
    }
};

#endif // ADMINWINDOW_H
