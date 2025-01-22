#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>

#include "dialogs\addstoredialog.h"

#define API_KEY "SecretKey"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    enum ArticlesTableColumns {
        ID_COLUMN = 0,
        NAME_COLUMN,
        DESCRIPTION_COLUMN,
        PRICE_COLUMN,
        TOTAL_IN_SHELF_COLUMN,
        TOTAL_IN_VAULT_COLUMN
    };

signals:
    void readingStoresFinished();

private slots:
    void readStores();
    void finishReadingStores();
    void readArticles();
    void finishReadingArticles();

    void on_idComboBox_activated(int index);
    void on_AddStorePushButton_clicked();

    // Add store slots.
    void acceptAddStoreSlot(QString id, QString name, QString address);

    // After POST.
    void onFinishPost(QNetworkReply *reply);

private:
    Ui::MainWindow* ui;
    AddStoreDialog* addStoreDialog;

    QNetworkAccessManager* netManager;
    QNetworkReply* netReply;
    QByteArray dataBuffer;
    qint64 currentStoreId;

    void requestArticles();
};
#endif // MAINWINDOW_H
