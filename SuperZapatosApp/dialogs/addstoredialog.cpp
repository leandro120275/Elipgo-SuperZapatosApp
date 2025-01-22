#include "addstoredialog.h"
#include "ui_addstoredialog.h"

AddStoreDialog::AddStoreDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddStoreDialog)
{
    ui->setupUi(this);
}

AddStoreDialog::~AddStoreDialog()
{
    delete ui;
}

void AddStoreDialog::on_acceptButton_clicked()
{
    emit addStore(ui->idLineEdit->text(), ui->nameLineEdit->text(), ui->addressPlainTextEdit->toPlainText());
}


void AddStoreDialog::on_cancelButton_clicked()
{
    close();
}

