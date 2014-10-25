#ifndef DROPENABLEDLISTWIDGET_H
#define DROPENABLEDLISTWIDGET_H

#include <QListWidget>

class DropEnabledListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit DropEnabledListWidget(QWidget *parent = 0);



signals:

public slots:

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent * event);
    virtual void dropEvent(QDropEvent *event);
};

#endif // DROPENABLEDLISTWIDGET_H
