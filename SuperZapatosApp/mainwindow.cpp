#include <QMessageBox>
#include <QRegExp>
#include <QHttpMultiPart>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set size of articles table columns.
    ui->tableWidget->setColumnWidth(ID_COLUMN, 20);
    ui->tableWidget->setColumnWidth(NAME_COLUMN, 150);
    ui->tableWidget->setColumnWidth(DESCRIPTION_COLUMN, 300);
    ui->tableWidget->setColumnWidth(PRICE_COLUMN, 120);
    ui->tableWidget->setColumnWidth(TOTAL_IN_SHELF_COLUMN, 120);
    ui->tableWidget->setColumnWidth(TOTAL_IN_VAULT_COLUMN, 120);

    //ui->tableWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

    // This is the current store in the ComboBox.
    currentStoreId = -1;

    // Prepare for server request.
    netManager = new QNetworkAccessManager(this);
    netReply = nullptr;

    // Get stores registers to fill store-id list (QComboBox).
    QNetworkRequest netRequest{QUrl(QString("http://127.0.0.1:8282/services/stores"))};

    netReply = netManager->get(netRequest);
    connect(netReply, &QNetworkReply::readyRead, this, &MainWindow::readStores);
    connect(netReply, &QNetworkReply::finished, this, &MainWindow::finishReadingStores);
}

void MainWindow::readStores()
{
    dataBuffer.append(netReply->readAll());
}

void MainWindow::finishReadingStores()
{
    qDebug() << "MainWindow::finishReadingStores()-->";

    if (netReply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error : " << netReply->errorString();
        QMessageBox::warning(this, "Error", QString("Request[Error] : %1").arg(netReply->errorString()));
    }
    else
    {
        // Convert the data from JSON document to a JSON object.
        QJsonObject jsonObject = QJsonDocument::fromJson(dataBuffer).object();

        QJsonArray jsonArray = jsonObject.value("PLURAL_MODEL_NAME").toArray();
        qDebug() << "MainWindow::finishReadingStores()-->jsonArraySize = " << jsonArray.size();

        // Fill store-id list.
        bool initialStore = true;
        for (const QJsonValue& value : jsonArray)
        {
            QJsonObject obj = value.toObject();
            qint64 id = obj.value("id").toInt();
            QString idString = QString::number(id);
            qDebug() << "MainWindow::finishReadingStores()-->id = " << idString;

            ui->idComboBox->addItem(idString);

            // Fill store fields.
            if (initialStore)
            {
                // Store fields.
                ui->nameTextLabel->setText(obj.value("name").toString());
                ui->addressTextLabel->setText(obj.value("address").toString());
                initialStore = false;
                currentStoreId = id;
            }
        }
    }
    netReply = nullptr;
    dataBuffer.clear();
    // Get articles for current store.
    requestArticles();
}

void MainWindow::requestArticles()
{
    if (currentStoreId > 0)
    {
        // Fill articles table (QTableWidget) for current store.
        QNetworkRequest netRequest{QUrl(QString("http://127.0.0.1:8282/services/store-articles/%1").arg(currentStoreId))};

        netReply = netManager->get(netRequest);
        connect(netReply,&QNetworkReply::readyRead,this,&MainWindow::readArticles);
        connect(netReply,&QNetworkReply::finished,this,&MainWindow::finishReadingArticles);
    }
}

void MainWindow::readArticles()
{
    dataBuffer.append(netReply->readAll());
}

void MainWindow::finishReadingArticles()
{
    qDebug() << "MainWindow::finishReadingArticles()-->";

    if (netReply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error : " << netReply->errorString();
        QMessageBox::warning(this, "Error", QString("Request[Error] : %1").arg(netReply->errorString()));
    }
    else
    {
        // Convert the data from JSON document to a JSON object.
        QJsonObject jsonObject = QJsonDocument::fromJson(dataBuffer).object();

        QJsonArray jsonArray = jsonObject.value("PLURAL_MODEL_NAME").toArray();
        qDebug() << "MainWindow::finishReadingArticles()-->jsonArraySize = " << jsonArray.size();

        ui->tableWidget->setRowCount(jsonArray.size());
        // Fill articles table.
        int row = 0;
        for (const QJsonValue& value : jsonArray)
        {
            QJsonObject obj = value.toObject();

            QString id = QString::number(obj.value("id").toInt());
            QString name = obj.value("name").toString();
            QString description = obj.value("description").toString();
            QString price = QString::number(obj.value("price").toDouble());
            QString total_in_shelf = QString::number(obj.value("total_in_shelf").toInt());
            QString total_in_vault = QString::number(obj.value("total_in_vault").toInt());

            qDebug() << "MainWindow::finishReadingArticles()-->id = " << id;
            qDebug() << "MainWindow::finishReadingArticles()-->name = " << name;
            qDebug() << "MainWindow::finishReadingArticles()-->description = " << description;
            qDebug() << "MainWindow::finishReadingArticles()-->price = " << price;
            qDebug() << "MainWindow::finishReadingArticles()-->total_in_shelf = " << total_in_shelf;
            qDebug() << "MainWindow::finishReadingArticles()-->total_in_vault = " << total_in_vault;

            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(id));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(name));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(description));
            ui->tableWidget->setItem(row, 3, new QTableWidgetItem(price));
            ui->tableWidget->setItem(row, 4, new QTableWidgetItem(total_in_shelf));
            ui->tableWidget->setItem(row, 5, new QTableWidgetItem(total_in_vault));
            row++;
        }
    }
    netReply = nullptr;
    dataBuffer.clear();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_idComboBox_activated(int index)
{
    currentStoreId = (ui->idComboBox->itemText(index)).toInt();

    ui->tableWidget->clearContents();
    // Get articles for current store.
    requestArticles();
}


void MainWindow::on_AddStorePushButton_clicked()
{
    addStoreDialog = new AddStoreDialog(this);

    connect(addStoreDialog, &AddStoreDialog::addStore, this, &MainWindow::acceptAddStoreSlot);

    addStoreDialog->show();
}

void MainWindow::acceptAddStoreSlot(QString id, QString name, QString address)
{
    //Validate captured data.
    qDebug() << "MainWindow::acceptAddStoreSlot-->Validating Add Store data...";

    // a digit (\d), one or more times (+)
    QRegExp re("\\d+");
    if (!re.exactMatch(id))
    {
        qDebug() << "Store id isn't a number!";
        QMessageBox::critical(this, "Data Validation", "Store id isn't a valid number!");
        return;
    }

    if(name.isEmpty())
    {
        QMessageBox::critical(this, "Data Validation", "Name field is empty!");
        return;
    }

    QNetworkRequest request;
    request.setUrl(QUrl("http://127.0.0.1:8282/services/store"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // API - KEY header.
    request.setRawHeader(QByteArray("api_key"), QByteArray(API_KEY));

    QJsonObject json;
    json.insert("id", id.toInt());
    json.insert("name", name);
    json.insert("address", address);

    QJsonDocument doc(json);
    QByteArray postData = doc.toJson();

    qDebug() << "postData: " << postData;

    netManager->post(request, postData);

    connect(netManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinishPost(QNetworkReply*)));

}

void MainWindow::onFinishPost(QNetworkReply *reply)
{
    // Disconnec netManager signal connection to avoid calling this slot for other requests.
    disconnect(netManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinishPost(QNetworkReply*)));

    addStoreDialog->close();
    QMessageBox::information(this, "Add Store", "Store was added successfully!");

    dataBuffer.append(reply->readAll());
    qDebug() << dataBuffer;

    QJsonObject jsonObject = QJsonDocument::fromJson(dataBuffer).object();
    qint64 id = jsonObject.value("id").toInt();
    QString idString = QString::number(id);
    ui->idComboBox->addItem(idString);


    dataBuffer.clear();
}
