#include "vpkcompare.h"

#include <iostream>

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QTextStream>

#include "ignoredfilesdialog.h"

VpkCompare::VpkCompare(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
    vpkexe_process_(NULL),
    vpkfile_number_(0)
{
    ui.setupUi(this);

    vpkexe_process_ = new QProcess(this);
    bool b = connect(vpkexe_process_,
                     SIGNAL(finished(int, QProcess::ExitStatus)),
                     this,
                     SLOT(onVpkExeProcessFinished(int, QProcess::ExitStatus)));
    Q_ASSERT(b);

    QSettings settings("settings.ini", QSettings::IniFormat, this);
    QString vpkexe = "E:\\Games\\Steam\\SteamApps\\common\\nuclear dawn\\bin"
                     "\\vpk.exe";
    ui.vpkexe_path->setText(settings.value("VPKEXE_PATH", vpkexe).toString());
    this->resize(settings.value("SIZE",
                                QSize(this->width(), this->height())).toSize());
}

VpkCompare::~VpkCompare()
{
    QSettings settings("settings.ini", QSettings::IniFormat, this);
    settings.setValue("VPKEXE_PATH", ui.vpkexe_path->text());
    settings.setValue("SIZE", this->size());
}

void VpkCompare::on_browse_vpkexe_clicked()
{
    QString default_dir_path;;
    QFileInfo fi(ui.vpkexe_path->text());
    if (fi.exists())
    {
        default_dir_path = fi.absolutePath();
    }
    else 
    {
        default_dir_path = QDir::homePath();
    }

    QFileDialog d;
    QString path = d.getOpenFileName(this, tr("Select vpk.exe file"),
                                     default_dir_path,
                                     tr("Executable (vpk.exe);;All files (*.*)"));

    if (!path.isEmpty())
        ui.vpkexe_path->setText(QDir::toNativeSeparators(path));
}

void VpkCompare::on_addVpkButton_clicked()
{
    QString default_dir_path = QDir::homePath();

    QFileDialog d;
    QStringList files = d.getOpenFileNames(this, tr("Select one or more vpk "
        "files"), default_dir_path, tr("VPK files (*.vpk);;All files (*.*)"));

    for (int i = 0; i < files.size(); ++i)
    {
        if (!files[i].isEmpty() && QFile::exists(files[i]))
            ui.vpkListWidget->addItem(QDir::toNativeSeparators(files[i]));
    }
}

void VpkCompare::on_removeVpkButton_clicked()
{
    delete ui.vpkListWidget->takeItem(ui.vpkListWidget->currentRow());
}

void VpkCompare::on_compare_clicked()
{
    if (ui.vpkListWidget->count() < 2)
    {
        QMessageBox::warning(this, tr("Not enough VPK files added"),
                             tr("Please add 2 or more VPK files for "
                                "comparison. Aborting "));
        return;
    }

    QStringList check_list;
    check_list << ui.vpkexe_path->text();
    for (int i = 0; i < ui.vpkListWidget->count(); ++i)
    {
        check_list << ui.vpkListWidget->item(i)->text();
    }

    if (!filesExist(check_list))
    {
        return;
    }

    runVpkExe();
}

void VpkCompare::on_ignore_clicked()
{
    IgnoredFilesDialog dlg(this, ignore_list_);
    if (QDialog::Accepted == dlg.exec())
    {
        ignore_list_ = dlg.ignoreList();
        if (ignore_list_.empty())
        {
            QFont font = ui.ignore->font();
            font.setBold(false);
            ui.ignore->setFont(font);
        }
        else
        {
            QFont font = ui.ignore->font();
            font.setBold(true);
            ui.ignore->setFont(font);
        }
    }
}

void VpkCompare::onVpkExeProcessFinished(int exitCode,
                                         QProcess::ExitStatus exitStatus)
{
    Q_ASSERT(vpkexe_process_);
    QByteArray standard_output = vpkexe_process_->readAllStandardOutput();
    QByteArray error_output = vpkexe_process_->readAllStandardError();

    QString status = exitStatus == QProcess::NormalExit ? "Normal" : "Crashed";

    log(tr("VPK.exe finished with status: %1, exit code: %2").arg(status).arg(exitCode));
    //log(tr("VPK.exe standard output: "));
    //log(standard_output);
    if (error_output.size() > 0)
    {
        log(tr("VPK.exe error output: "));
        log(error_output);
    }

    // parse output
    vpk_files_info_.push_back(VpkFileInfo());
    VpkFileInfo &fi = vpk_files_info_[vpk_files_info_.size() - 1];
    fi.vpkfilepath = ui.vpkListWidget->item(vpkfile_number_ - 1)->text();

    QTextStream vpk_output_stream(standard_output, QIODevice::ReadOnly);
    while (!vpk_output_stream.atEnd())
    {
        QString line = vpk_output_stream.readLine();
        QString::SectionFlag flag = QString::SectionSkipEmpty;
        fi.pathes.push_back(line.section(' ', 0, 0, flag));
        fi.crcs.push_back(line.section(' ', 1, 1, flag).remove(0, 6));
    }

    // run next vpk.exe or comparison if all vpk files were parsed
    if (vpkfile_number_ < ui.vpkListWidget->count())
        runVpkExe();
    else
    {
        for (int i = 0; i < vpk_files_info_.size() - 1; ++i)
        {
            for (int k = i + 1; k < vpk_files_info_.size(); ++k)
            {
                compare(vpk_files_info_[i], vpk_files_info_[k]);
            }
        }

        // perform cleanup
        vpk_files_info_.clear();
        vpkfile_number_ = 0;
    }
}


/*
void VpkCompare::on_embed_clicked() 
{
    QStringList check_list;
    check_list << ui.vpkexe_path->text() << ui.bspfile_path->text() << ui.data_folder_path->text();
    if (!filesExist(check_list))
    {
        return;
    }

    QFile bspzip_filelist_file(QDir::homePath() + "/bspzip_filelist.txt"); 
    log(tr("Opening file for writing: %1").arg( bspzip_filelist_file.fileName() ));
    if (!bspzip_filelist_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        log(tr("Failed to open file for writing: %1").arg(bspzip_filelist_file.fileName()));
        return;
    }

    QDir data_dir( ui.data_folder_path->text(), "", QDir::Name | QDir::IgnoreCase, QDir::AllDirs | QDir::Files|  QDir::NoDotAndDotDot );
    getDataFolderFiles(data_dir, &data_folder_files_);

    if (data_folder_files_.isEmpty())
    {
        log(tr("No files found for embedding in directory: %1 Aborting...").arg(data_dir.absolutePath()));
        return;
    }

    log("Adding file list into filelist.txt...");

    QTextStream out(&bspzip_filelist_file);
    QString absolute_file_path;
    Q_FOREACH(absolute_file_path, data_folder_files_)
    {
        absolute_file_path = QDir::toNativeSeparators(absolute_file_path);
        QString relative_file_path = absolute_file_path.mid(ui.data_folder_path->text().size() + 1);

        log(relative_file_path);
        log(absolute_file_path);

        // no quotes here as BSPZIP doesn't handle them
        out << relative_file_path << "\n";
        out << absolute_file_path << "\n";
    }
    data_folder_files_.clear(); // must be cleared! or we get bugs

    bspzip_filelist_file.close();
    log(tr("File bspzip_filelist.txt was successfully created"));

    QString bsp_file_path = QDir::toNativeSeparators(ui.bspfile_path->text());
    QString filelist_path = QDir::toNativeSeparators(bspzip_filelist_file.fileName());
    QStringList process_arguments;
    process_arguments << "-addorupdatelist";
    process_arguments << bsp_file_path;
    process_arguments << filelist_path;
    process_arguments << bsp_file_path;

//     if (bspzip_process_) 
//     {
//         delete bspzip_process_;
//     }

    log(tr("Starting BSPZIP.EXE with following arguments"));
    Q_FOREACH(QString argument, process_arguments)
    {
        log(argument);
    }

    bspzip_process_->start(ui.vpkexe_path->text(), process_arguments);
}

void VpkCompare::on_extract_clicked() 
{
    QString bsp_file_path = QDir::toNativeSeparators(ui.bspfile_path->text());
    QString data_folder = QDir::toNativeSeparators(ui.data_folder_path->text());

    QStringList check_list;
    check_list << ui.vpkexe_path->text() << bsp_file_path << data_folder;
    if (!filesExist(check_list))
    {
        return;
    }
    
    QStringList process_arguments;
    process_arguments << "-extractfiles";
    process_arguments << bsp_file_path; 
    process_arguments << data_folder; 

//     if (bspzip_process_) 
//     {
//         delete bspzip_process_;
//     }

//     bspzip_process_ = new QProcess(this);
//     bool b = connect(bspzip_process_, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onBspZipProcessFinished(int, QProcess::ExitStatus)));
//    Q_ASSERT(b);

    bspzip_process_->start(ui.vpkexe_path->text(), process_arguments);
}
//*/

void VpkCompare::log(const QString &logstr) 
{
    ui.console->appendPlainText(logstr);
    //std::cout << logstr.toLatin1().constData() << std::endl;
}


bool VpkCompare::filesExist(const QStringList &files) 
{
    Q_FOREACH(QString file_path, files)
    {
        QFileInfo fi(file_path);
        if (!fi.exists())
        {
            QMessageBox::warning(this, tr("File Not Found"), tr("The file doesn't exist: %1. Aborting ").arg(file_path));
            return false;
        }
    }
    return true;
}

void VpkCompare::runVpkExe()
{
    if (vpkfile_number_ >= ui.vpkListWidget->count())
        return;

    QStringList process_arguments;
    process_arguments << "L";
    process_arguments << ui.vpkListWidget->item(vpkfile_number_)->text();
    ++vpkfile_number_;

    vpkexe_process_->start(ui.vpkexe_path->text(), process_arguments);
}

void VpkCompare::compare(const VpkFileInfo &a, const VpkFileInfo &b)
{
    bool first_time = true;
    bool check_crc = ui.crcCheckBox->isChecked();
    int conflict_counter = 0;
    int same_crc_counter = 0;
    int ignored_counter = 0;

    for (int i = 0; i < a.pathes.size(); ++i)
    {
        for (int k = 0; k < b.pathes.size(); ++k)
        {
            if (a.pathes[i] == b.pathes[k])
            {
                if (check_crc)
                {
                    // skip files with same crc
                    if (a.crcs[i] == b.crcs[k])
                    {
                        ++same_crc_counter;
                        continue;
                    }
                }

                // check ignores
                if (ignore_list_.size() > 0)
                {
                    bool must_be_ignored = false;
                    for (int q = 0; q < ignore_list_.size(); ++q)
                    {
                        QRegExp exp(ignore_list_.at(q), Qt::CaseInsensitive, QRegExp::Wildcard);
                        if (exp.exactMatch(a.pathes[i]))
                        {
                            ++ignored_counter;
                            must_be_ignored = true;
                            break;
                        }
                    }
                    if (must_be_ignored)
                        continue;
                }

                if (first_time)
                {
                    log(tr("Collisions between:\n %1 \n %2").arg(a.vpkfilepath).arg(b.vpkfilepath));
                    first_time = false;
                }
                log(a.pathes[i]);
                ++conflict_counter;
            }
        }
    }
    if (conflict_counter)
        log(tr("%1 files conflicted, %2 with same crc, %3 "
               "ignored").arg(conflict_counter).arg(same_crc_counter).
            arg(ignored_counter));
}

