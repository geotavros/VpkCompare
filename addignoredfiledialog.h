#ifndef ADDIGNOREDFILEDIALOG_H
#define ADDIGNOREDFILEDIALOG_H

#include <QDialog>

namespace Ui {
class AddIgnoredFileDialog;
}

class AddIgnoredFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddIgnoredFileDialog(QWidget *parent = 0);
    ~AddIgnoredFileDialog();

    QString filename() const;

private:
    Ui::AddIgnoredFileDialog *ui;
};

#endif // ADDIGNOREDFILEDIALOG_H
