#include "mainwindow.h"
#include <QApplication>
#include <QtSql>

QSqlDatabase db;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // nawiąż połącznie z bazą danych
    QString ServerName = "LOCALHOST\\SQLEXPRESS";
    QString DBName = "zz_sql_project";
    db = QSqlDatabase::addDatabase("QODBC3");
    db.setConnectOptions();

    QString dsn = QString("DRIVER={SQL Server};Server=%1;Database=%2;Trusted_Connection=Yes;").arg(ServerName).arg(DBName);
    db.setDatabaseName(dsn);
    //


    MainWindow w;
    w.show();

    return a.exec();
}
