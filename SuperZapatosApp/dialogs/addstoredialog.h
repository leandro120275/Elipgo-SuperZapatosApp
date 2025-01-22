#ifndef ADDSTOREDIALOG_H
#define ADDSTOREDIALOG_H

#include <QDialog>

namespace Ui {
class AddStoreDialog;
}

class AddStoreDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddStoreDialog(QWidget *parent = nullptr);
    ~AddStoreDialog();

signals:
    void addStore(QString id, QString name, QString address);

private slots:
    void on_acceptButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::AddStoreDialog *ui;
};

#endif // ADDSTOREDIALOG_H
