#include "dropenabledlistwidget.h"

#include <iostream>

#include <QDir>
#include <QDragEnterEvent>
#include <QEvent>

#include <QMimeData>

DropEnabledListWidget::DropEnabledListWidget(QWidget *parent) :
    QListWidget(parent)
{
    setAcceptDrops(true);
}

void DropEnabledListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    QStringList formats = event->mimeData()->formats();
    for (int i = 0; i < formats.size(); ++i)
    {
       // std::cout << formats[i].toLatin1().data() << std::endl;
    }//*/
    if (event->mimeData()->hasFormat("application/x-qt-windows-mime;value=\"FileNameW\""))
    {
        event->acceptProposedAction();
        //std::cout << "accepted x-qt-windows-mime proposed action" << std::endl;
    }
    else if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
        //std::cout << "accepted proposed action" << std::endl;
    }
}

void DropEnabledListWidget::dragMoveEvent(QDragMoveEvent * event)
{
    if (event->mimeData()->hasFormat("application/x-qt-windows-mime;value=\"FileNameW\""))
    {
        event->acceptProposedAction();
        //std::cout << "accepted proposed action" << std::endl;
    }
    else if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
        //std::cout << "accepted proposed action" << std::endl;
    }
}

void DropEnabledListWidget::dropEvent(QDropEvent *event)
{
    //textBrowser->setPlainText(event->mimeData()->text());
    //mimeTypeCombo->clear();
    //mimeTypeCombo->addItems(event->mimeData()->formats());

    QList<QUrl> urls = event->mimeData()->urls();
    for (int i = 0; i < urls.size(); ++i)
    {
        if (urls[i].isLocalFile())
        {
            QListWidgetItem *item = new QListWidgetItem(QDir::toNativeSeparators(urls[i].toLocalFile()));
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            this->addItem(item);
        }
        //std::cout << urls[i].fileName().toLatin1().data() << std::endl;
    }

    //std::cout << .data() << std::endl;
    //this->addItem(QString::fromWCharArray((wchar_t*)event->mimeData()->data("application/x-qt-windows-mime;value=\"FileNameW\"").data()));

    event->acceptProposedAction();
}

