#include <QCoreApplication>
#include <QtHttpServer>
#include <QSqlRecord>

#include "db-helper/QSQLDbHelper.h"
#include "entities/ArticleEntity.h"
#include "entities/StoreEntity.h"

#define API_KEY "SecretKey"

static bool checkApiKeyHeader(const QList<QPair<QByteArray, QByteArray>> &headers)
{
    for (const auto &[key, value] : headers) {
        if (key == "api_key" && value == API_KEY) {
            return true;
        }
    }
    return false;
}

static std::optional<QJsonObject> byteArrayToJsonObject(const QByteArray &arr)
{
    QJsonParseError err;
    const auto json = QJsonDocument::fromJson(arr, &err);
    if (err.error || !json.isObject())
        return std::nullopt;
    return json.object();
}

int main(int argc, char *argv[])
{
    qDebug() << "Compiled with Qt Version = " << QT_VERSION_STR;

    QCoreApplication a(argc, argv);

    // Setting httpServer configurations.
    QHttpServer httpServer;
    const auto port = httpServer.listen(QHostAddress::Any, 8282);

    if (!port)
    {
        qDebug() << "Server ports not available!";
        return 0;
    }

    // Database driver name.
    const char* driverName = "QPSQL";
    // Connect to database.
    QSQLDbHelper* qSQLDbHelper = new QSQLDbHelper(driverName);
    //QSqlDatabase* db = qSQLDbHelper->connect("localhost", "db_superZapatos", "soporte", "soporte7!");
    qSQLDbHelper->connect("localhost", "db_superZapatos", "soporte", "soporte7!");

    httpServer.route("/", []() {
        return "Hello world";
    });

    // Get the articles of all stores.
    httpServer.route("/services/articles", QHttpServerRequest::Method::Get,
                     [qSQLDbHelper](const QHttpServerRequest&) {

                        QJsonObject mainJsonObject;

                        if (qSQLDbHelper->getDB()->open())
                        {
                            QSqlQuery* query = new QSqlQuery(*(qSQLDbHelper->getDB()));

                            query->setForwardOnly(true);
                            // Select all from articles table
                            if (!query->prepare(QString("SELECT * from articles")))
                            {
                                qDebug() <<"Error = " << qSQLDbHelper->getDB()->lastError().text();

                                mainJsonObject.insert("success", "false");
                                mainJsonObject.insert("error_code", "500");
                                mainJsonObject.insert("error_ms", "Server Error");

                                return QHttpServerResponse(mainJsonObject);
                            }

                            int queryResultRowCount = qSQLDbHelper->selectRowCountResult(query);
                            qDebug() << "Initial Row Count = " << queryResultRowCount;

                            if (!qSQLDbHelper->executeSelect(query))
                            {
                                mainJsonObject.insert("success", "false");
                                mainJsonObject.insert("error_code", "500");
                                mainJsonObject.insert("error_ms", "Server Error");

                                return QHttpServerResponse(mainJsonObject);
                            }

                            mainJsonObject.insert("success", "true");

                            QSqlRecord record = query->record();
                            qDebug() << "Number of columns: " << record.count();

                            ArticleEntity articleEntity;

                            int idCol           = record.indexOf("id");                // index of the field "id"
                            int nameCol         = record.indexOf("name");              // index of the field "name"
                            int descriptionCol  = record.indexOf("description");       // index of the field "description"
                            int priceCol        = record.indexOf("price");             // index of the field "price"
                            int totalInShelfCol = record.indexOf("total_in_shelf");    // index of the field "total_in_shelf"
                            int totalInVaultCol = record.indexOf("total_in_vault");    // index of the field "total_in_vault"
                            int storeIdCol      = record.indexOf("store_id");          // index of the field "store_id"

                            QJsonArray arrayObject;

                            while (query->next())
                            {
                                qDebug() << query->value(idCol).toInt() << " "
                                         << query->value(nameCol).toString() << " "
                                         << query->value(descriptionCol).toString() << " "
                                         << query->value(priceCol).toDouble() << " "
                                         << query->value(totalInShelfCol).toInt() << " "
                                         << query->value(totalInVaultCol).toInt() << " "
                                         << query->value(storeIdCol).toInt() << " ";

                                articleEntity.setId(query->value(idCol).toInt());
                                articleEntity.setName(query->value(nameCol).toString());
                                articleEntity.setDescription(query->value(descriptionCol).toString());
                                articleEntity.setPrice(query->value(priceCol).toDouble());
                                articleEntity.setTotal_in_shelf(query->value(totalInShelfCol).toInt());
                                articleEntity.setTotal_in_vault(query->value(totalInVaultCol).toInt());
                                articleEntity.setStore_id(query->value(storeIdCol).toInt());

                                QJsonObject jsonObject = articleEntity.toJson();
                                arrayObject.append(jsonObject);
                            }

                            mainJsonObject.insert("PLURAL_MODEL_NAME", arrayObject);

                        }
                        else
                        {
                            qDebug() << "Something went wrong with database connection:" << qSQLDbHelper->getDB()->lastError().text();
                        }

                        return QHttpServerResponse(mainJsonObject);
                     });

    // Get all the articles of a specific store.
    httpServer.route("/services/store-articles/<arg>", QHttpServerRequest::Method::Get,
                     [qSQLDbHelper](qint64 storeId, const QHttpServerRequest &) {

                         QJsonObject mainJsonObject;

                         if (qSQLDbHelper->getDB()->open())
                         {
                             QSqlQuery* query = new QSqlQuery(*(qSQLDbHelper->getDB()));

                             query->setForwardOnly(true);
                             // Select all from articles table
                             QString statement = QString("SELECT * FROM articles WHERE store_id = %1").arg(storeId);
                             qDebug() << statement;

                             if (!query->prepare(statement))
                             {
                                 qDebug() <<"Error = " << qSQLDbHelper->getDB()->lastError().text();

                                 mainJsonObject.insert("success", "false");
                                 mainJsonObject.insert("error_code", "500");
                                 mainJsonObject.insert("error_ms", "Server Error");

                                 return QHttpServerResponse(mainJsonObject);
                             }

                             int queryResultRowCount = qSQLDbHelper->selectRowCountResult(query);
                             qDebug() << "Initial Row Count = " << queryResultRowCount;

                             if (!qSQLDbHelper->executeSelect(query))
                             {
                                 mainJsonObject.insert("success", "false");
                                 mainJsonObject.insert("error_code", "500");
                                 mainJsonObject.insert("error_ms", "Server Error");

                                 return QHttpServerResponse(mainJsonObject);
                             }

                             QSqlRecord record = query->record();
                             qDebug() << "Number of columns: " << record.count();

                             mainJsonObject.insert("success", "true");

                             ArticleEntity articleEntity;

                             int idCol           = record.indexOf("id");                // index of the field "id"
                             int nameCol         = record.indexOf("name");              // index of the field "name"
                             int descriptionCol  = record.indexOf("description");       // index of the field "description"
                             int priceCol        = record.indexOf("price");             // index of the field "price"
                             int totalInShelfCol = record.indexOf("total_in_shelf");    // index of the field "total_in_shelf"
                             int totalInVaultCol = record.indexOf("total_in_vault");    // index of the field "total_in_vault"
                             int storeIdCol      = record.indexOf("store_id");          // index of the field "store_id"

                             QJsonArray arrayObject;

                             while (query->next())
                             {
                                 qDebug() << query->value(idCol).toInt() << " "
                                          << query->value(nameCol).toString() << " "
                                          << query->value(descriptionCol).toString() << " "
                                          << query->value(priceCol).toDouble() << " "
                                          << query->value(totalInShelfCol).toInt() << " "
                                          << query->value(totalInVaultCol).toInt() << " "
                                          << query->value(storeIdCol).toInt() << " ";

                                 articleEntity.setId(query->value(idCol).toInt());
                                 articleEntity.setName(query->value(nameCol).toString());
                                 articleEntity.setDescription(query->value(descriptionCol).toString());
                                 articleEntity.setPrice(query->value(priceCol).toDouble());
                                 articleEntity.setTotal_in_shelf(query->value(totalInShelfCol).toInt());
                                 articleEntity.setTotal_in_vault(query->value(totalInVaultCol).toInt());
                                 articleEntity.setStore_id(query->value(storeIdCol).toInt());

                                 QJsonObject jsonObject = articleEntity.toJson();
                                 arrayObject.append(jsonObject);
                             }

                             mainJsonObject.insert("PLURAL_MODEL_NAME", arrayObject);
                         }
                         else
                         {
                             qDebug() << "Something went wrong with database connection:" << qSQLDbHelper->getDB()->lastError().text();
                         }

                         return QHttpServerResponse(mainJsonObject);
                     });

    // Get all the stores.
    httpServer.route("/services/stores", QHttpServerRequest::Method::Get,
                     [qSQLDbHelper](const QHttpServerRequest&) {

                         QJsonObject mainJsonObject;

                        if (qSQLDbHelper->getDB()->open())
                         {
                             QSqlQuery* query = new QSqlQuery(*(qSQLDbHelper->getDB()));

                             query->setForwardOnly(true);
                             // Select all from articles table
                             if (!query->prepare(QString("SELECT * from stores")))
                             {
                                 qDebug() <<"Error = " << qSQLDbHelper->getDB()->lastError().text();

                                 mainJsonObject.insert("success", "false");
                                 mainJsonObject.insert("error_code", "500");
                                 mainJsonObject.insert("error_ms", "Server Error");

                                 return QHttpServerResponse(mainJsonObject);
                             }

                             int queryResultRowCount = qSQLDbHelper->selectRowCountResult(query);
                             qDebug() << "Initial Row Count = " << queryResultRowCount;

                             if (!qSQLDbHelper->executeSelect(query))
                             {
                                 mainJsonObject.insert("success", "false");
                                 mainJsonObject.insert("error_code", "500");
                                 mainJsonObject.insert("error_ms", "Server Error");

                                 return QHttpServerResponse(mainJsonObject);
                             }

                             mainJsonObject.insert("success", "true");

                             QSqlRecord record = query->record();
                             qDebug() << "Number of columns: " << record.count();

                             StoreEntity storeEntity;

                             int idCol       = record.indexOf("id");        // index of the field "id"
                             int nameCol     = record.indexOf("name");      // index of the field "name"
                             int addressCol  = record.indexOf("address");   // index of the field "description"

                             QJsonArray arrayObject;

                             while (query->next())
                             {
                                 qDebug() << query->value(idCol).toInt() << " "
                                          << query->value(nameCol).toString() << " "
                                          << query->value(addressCol).toString() << " ";

                                 storeEntity.setId(query->value(idCol).toInt());
                                 storeEntity.setName(query->value(nameCol).toString());
                                 storeEntity.setAddress(query->value(addressCol).toString());

                                 QJsonObject jsonObject = storeEntity.toJson();
                                 arrayObject.append(jsonObject);
                             }

                             mainJsonObject.insert("PLURAL_MODEL_NAME", arrayObject);

                         }
                         else
                         {
                             qDebug() << "Something went wrong with database connection:" << qSQLDbHelper->getDB()->lastError().text();
                         }

                         return QHttpServerResponse(mainJsonObject);
                     });

    // Get a specific store.
    httpServer.route("/services/stores/<arg>", QHttpServerRequest::Method::Get,
                     [qSQLDbHelper](qint64 storeId, const QHttpServerRequest &) {

                         QJsonObject mainJsonObject;

                         if (qSQLDbHelper->getDB()->open())
                         {
                             QSqlQuery* query = new QSqlQuery(*(qSQLDbHelper->getDB()));

                             query->setForwardOnly(true);
                             // Select all from articles table
                             QString statement = QString("SELECT * FROM stores WHERE id = %1").arg(storeId);
                             qDebug() << statement;

                             if (!query->prepare(statement))
                             {
                                 qDebug() <<"Error = " << qSQLDbHelper->getDB()->lastError().text();

                                 mainJsonObject.insert("success", "false");
                                 mainJsonObject.insert("error_code", "500");
                                 mainJsonObject.insert("error_ms", "Server Error");

                                 return QHttpServerResponse(mainJsonObject);
                             }

                             int queryResultRowCount = qSQLDbHelper->selectRowCountResult(query);
                             qDebug() << "Initial Row Count = " << queryResultRowCount;

                             if (!qSQLDbHelper->executeSelect(query))
                             {
                                 mainJsonObject.insert("success", "false");
                                 mainJsonObject.insert("error_code", "500");
                                 mainJsonObject.insert("error_ms", "Server Error");

                                 return QHttpServerResponse(mainJsonObject);
                             }

                             QSqlRecord record = query->record();
                             qDebug() << "Number of columns: " << record.count();

                             if(query->next())
                             {
                                 mainJsonObject.insert("success", "true");

                                 StoreEntity storeEntity;

                                 int idCol       = record.indexOf("id");        // index of the field "id"
                                 int nameCol     = record.indexOf("name");      // index of the field "name"
                                 int addressCol  = record.indexOf("address");   // index of the field "description"

                                 qDebug() << query->value(idCol).toInt() << " "
                                          << query->value(nameCol).toString() << " "
                                          << query->value(addressCol).toString() << " ";

                                 storeEntity.setId(query->value(idCol).toInt());
                                 storeEntity.setName(query->value(nameCol).toString());
                                 storeEntity.setAddress(query->value(addressCol).toString());

                                 QJsonObject jsonObject = storeEntity.toJson();

                                 mainJsonObject.insert("MODEL_NAME", jsonObject);
                             }
                             else
                             {
                                 mainJsonObject.insert("success", "false");
                                 mainJsonObject.insert("error_code", "404");
                                 mainJsonObject.insert("error_ms", "Record not found");

                                 return QHttpServerResponse(mainJsonObject);
                             }
                         }
                         else
                         {
                             qDebug() << "Something went wrong with database connection:" << qSQLDbHelper->getDB()->lastError().text();
                         }

                         return QHttpServerResponse(mainJsonObject);
                     });

    // Post a new store.
    httpServer.route(
        "/services/store", QHttpServerRequest::Method::Post,
        [qSQLDbHelper](const QHttpServerRequest &request) {

            if (!checkApiKeyHeader(request.headers()))
            {
                return QHttpServerResponse(QHttpServerResponder::StatusCode::Unauthorized);
            }

            QJsonObject mainJsonObject;
            // Convert byteArray to JSON object.
            const auto json = byteArrayToJsonObject(request.body());

            if (!json || !json->contains("id") || !json->contains("name") || !json->contains("address"))
                return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);

            // Insert record into database.
            if (qSQLDbHelper->getDB()->open())
            {
                QSqlQuery* query = new QSqlQuery(*(qSQLDbHelper->getDB()));

                query->setForwardOnly(true);
                // Build statement.
                QString statement = QString("INSERT INTO stores (id, name, address) VALUES (%1, '%2', '%3')")
                                    .arg(json->value("id").toInt())
                                    .arg(json->value("name").toString())
                                    .arg(json->value("address").toString());
                qDebug() << statement;

                if (!query->prepare(statement))
                {
                    qDebug() <<"Error = " << qSQLDbHelper->getDB()->lastError().text();

                    mainJsonObject.insert("success", "false");
                    mainJsonObject.insert("error_code", "500");
                    mainJsonObject.insert("error_ms", "Server Error");

                    return QHttpServerResponse(mainJsonObject);
                }

                if (!qSQLDbHelper->executeInsert(query))
                {
                    mainJsonObject.insert("success", "false");
                    mainJsonObject.insert("error_code", "500");
                    mainJsonObject.insert("error_ms", "Server Error");

                    return QHttpServerResponse(mainJsonObject);
                }

                StoreEntity storeEntity;

                storeEntity.setId(json->value("id").toInt());
                storeEntity.setName(json->value("name").toString());
                storeEntity.setAddress(json->value("address").toString());

                mainJsonObject = storeEntity.toJson();
            }
            else
            {
                qDebug() << "Something went wrong with database connection:" << qSQLDbHelper->getDB()->lastError().text();
            }

            return QHttpServerResponse(mainJsonObject, QHttpServerResponder::StatusCode::Created);
        });

    qDebug() << QCoreApplication::translate("QHttpServerExample",
                                            "Running on http://127.0.0.1:%1/").arg(port);

    /*
    // This must happen only when App is closed.
    qSQLDbHelper->disConnect();
    delete qSQLDbHelper;
    */

    return a.exec();
}
