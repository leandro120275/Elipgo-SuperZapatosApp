#ifndef QSQLDBHELPER_H
#define QSQLDBHELPER_H

#include <QtSql>
#include <QString>
#include <QMap>
#include <QDebug>

class QSQLDbHelper
{
private:
    QMap<int, QString> resulSet;
public:
    QSQLDbHelper(const char* driver);
    ~QSQLDbHelper();
    QSqlDatabase* connect(const QString& server,
                          const QString& databaseName,
                          const QString& userName,
                          const QString& password );
    void disConnect();

    int selectRowCountResult(QSqlQuery* query);
    bool executeSelect(QSqlQuery* query);
    bool executeInsert(QSqlQuery* query);
    bool executeUpdate(QSqlQuery* query);
    bool executeDelete(QSqlQuery* query);
    QSqlDatabase* getDB();

private:
    QSqlDatabase* db;
};

#endif // QSQLDBHELPER_H
