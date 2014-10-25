#include "ignoredfilesdialog.h"
#include "ui_ignoredfilesdialog.h"
#include "addignoredfiledialog.h"

IgnoredFilesDialog::IgnoredFilesDialog(QWidget *parent, QStringList ignore_list) :
    QDialog(parent),
    ui(new Ui::IgnoredFilesDialog)
{
    ui->setupUi(this);
    for (int i = 0; i < ignore_list.size(); ++i)
    {
        QListWidgetItem *item = new QListWidgetItem(ignore_list[i]);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->ignore_list->addItem(item);
    }
    //ui->ignore_list->insertItems(0, ignore_list);
}

IgnoredFilesDialog::~IgnoredFilesDialog()
{
    delete ui;
}

QStringList IgnoredFilesDialog::ignoreList() const
{
    QStringList result;
    for (int i = 0; i < ui->ignore_list->count(); ++i)
    {
        result.append(ui->ignore_list->item(i)->text());
    }
    return result;
}

void IgnoredFilesDialog::on_add_clicked()
{
    AddIgnoredFileDialog dialog(this);
    //connect(dialog_, SIGNAL(accepted()), this, SLOT(on_addignoredfiledialog_accepted()));
    if (QDialog::Accepted == dialog.exec())
    {
        if (!dialog.filename().isEmpty())
        {
            QListWidgetItem *item = new QListWidgetItem(dialog.filename());
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            ui->ignore_list->addItem(item);
            //ui->ignore_list->addItem(dialog.filename());
        }
    }
}

void IgnoredFilesDialog::on_remove_clicked()
{
    delete ui->ignore_list->takeItem(ui->ignore_list->currentRow());
}
