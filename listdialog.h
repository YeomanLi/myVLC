#ifndef LISTDIALOG_H
#define LISTDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QTextCodec>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include "modules/grouplist.h"
#include "widget/adddialog.h"

namespace Ui {
class listDialog;
}

class listDialog : public QDialog
{
    Q_OBJECT

public:
    explicit listDialog(QWidget *parent = 0);
    ~listDialog();
    void refreshList();
    void dfsFiles(const char * dirPath, QStringList & fileList);

signals:
//    void listPlay(QString, QString);
    void listPlay(ChannelGroup *, int);

public slots:
    void recordPos();
    void moveDif(QPoint);

    void selectChannel(QTreeWidgetItem *, int);
    void addGroupOrChannel(QString, QString);
    void addFromFile(QStringList);
    void delGroupOrChannel();
    void debug();

private:
    Ui::listDialog *ui;
    QPoint record;

    GroupList *currentList;

    addDialog *addWidget;
};

#endif // LISTDIALOG_H
