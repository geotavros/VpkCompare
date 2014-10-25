#ifndef BSPZIPGUI_H
#define BSPZIPGUI_H

#include <QDialog>
#include <QDir>
#include <QProcess>
#include <QString>

#include "ui_vpkcompare.h"

struct VpkFileInfo
{
    QVector<QString> pathes;    // file path
    QVector<QString> crcs;      // file crc
    QString vpkfilepath;        // full path to vpk file
};

class VpkCompare : public QMainWindow
{
    Q_OBJECT

public:
    VpkCompare(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~VpkCompare();

public slots:
    void on_browse_vpkexe_clicked();
    void on_addVpkButton_clicked();
    void on_removeVpkButton_clicked();
    void on_compare_clicked();
    void on_ignore_clicked();

//    void on_embed_clicked();
//    void on_extract_clicked();
    
    void onVpkExeProcessFinished(int exitCode, QProcess::ExitStatus exitStatus );


private:
    void log(const QString &logstr);
//    void getDataFolderFiles(const QDir &dir, QStringList *file_list);
    bool filesExist(const QStringList &files);
    void runVpkExe();
    void compare(const VpkFileInfo &a, const VpkFileInfo &b);
//    QStringList data_folder_files_;
    QProcess    *vpkexe_process_;

    Ui::VpkCompareClass ui;
    int vpkfile_number_;
    QVector<VpkFileInfo> vpk_files_info_;
    QStringList ignore_list_;
};

#endif // BSPZIPGUI_H
