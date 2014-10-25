#ifndef IGNOREDFILESDIALOG_H
#define IGNOREDFILESDIALOG_H

#include <QDialog>

class AddIgnoredFileDialog;

namespace Ui {
class IgnoredFilesDialog;
}

class IgnoredFilesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IgnoredFilesDialog(QWidget *parent, QStringList ignore_list);
    ~IgnoredFilesDialog();

    QStringList ignoreList() const;

private slots:
    void on_add_clicked();

    void on_remove_clicked();

private:
    Ui::IgnoredFilesDialog *ui;
};

#endif // IGNOREDFILESDIALOG_H
