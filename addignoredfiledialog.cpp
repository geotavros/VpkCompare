#include "addignoredfiledialog.h"
#include "ui_addignoredfiledialog.h"

AddIgnoredFileDialog::AddIgnoredFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddIgnoredFileDialog)
{
    ui->setupUi(this);
}

AddIgnoredFileDialog::~AddIgnoredFileDialog()
{
    delete ui;
}

QString AddIgnoredFileDialog::filename() const
{
    return ui->filename->text();
}
