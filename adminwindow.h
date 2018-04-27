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

    void on_MakeAdminPushButton_clicked();

    void on_CategoryComboBox_currentIndexChanged(const QString &arg1);

    void on_AddItemPushButton_clicked();

private:
    Ui::adminwindow *ui;

    // My Functions

    void SQLError(QString s = "")
    {
        QMessageBox ErrMsg;
        if(s.isEmpty()) ErrMsg.setText(db.lastError().text());
        else ErrMsg.setText(s);
        ErrMsg.exec();
    }

    int AddModel(QString, QString, QString, QString);
    int AddItem(QString, int, int);
    void ChangeQuantity(int, int);


};

#endif // ADMINWINDOW_H
