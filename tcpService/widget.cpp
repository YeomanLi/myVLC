#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    totalBytes = 0;
    bytesReceived = 0;
    fileNameSize = 0;

    loadSize = 4*1024;
    totalBytes = 0;
    bytesWritten = 0;
    bytesToWrite = 0;

    flagDown = false;

    connect(&tcpServer,&QTcpServer::newConnection,this, &Widget::acceptConnection);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void Widget::start()
{
    ui->startButton->setEnabled(false);
    bytesReceived =0;
    if(!tcpServer.listen(QHostAddress::LocalHost,6666))
    {
        qDebug() << tcpServer.errorString();
        close();
        return;
    }
    ui->serverStatusLabel->setText("����...");
}

void Widget::acceptConnection()
{
    tcpServerConnection = tcpServer.nextPendingConnection();

    connect(tcpServerConnection,&QIODevice::readyRead,
            this, &Widget::updateServerProgress);
    connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

    connect(tcpServerConnection,&QIODevice::bytesWritten,
            this, &Widget::updateClientProgress);

    ui->serverStatusLabel->setText("��������");
    tcpServer.close();
}


void Widget::updateClientProgress(qint64 numBytes)
{
    bytesWritten += (int)numBytes;
    if(bytesToWrite > 0)
    {
        outBlock = localFile->read(qMin(bytesToWrite,loadSize));
        bytesToWrite -= (int)tcpServerConnection->write(outBlock);
        outBlock.resize(0);
    }else{
        localFile->close();
    }

    ui->serverProgressBar->setMaximum(totalBytes);
    ui->serverProgressBar->setValue(bytesWritten);
    if(bytesWritten == totalBytes)
    {
        ui->serverStatusLabel->setText("�����ļ�" + wfileName + "�ɹ�");
        localFile->close();
        totalBytes = 0;
        bytesWritten = 0;
        bytesToWrite = 0;
        ui->serverProgressBar->setValue(0);

        ui->sendpushButton->setEnabled(true);
        ui->addFileButton->setEnabled(true);

        flagDown = false;
    }
}


void Widget::updateServerProgress()
{
    QDataStream in(tcpServerConnection);
    in.setVersion(QDataStream::Qt_4_6);

    if(bytesReceived <= sizeof(qint64)*2)
    {
        if((tcpServerConnection->bytesAvailable() >=
            sizeof(qint64)*2) && (fileNameSize == 0))
        {
            in >> totalBytes >> fileNameSize;
            bytesReceived += sizeof(qint64) * 2;
        }
        if((tcpServerConnection->bytesAvailable()
            >= fileNameSize) && (fileNameSize != 0))
            {
            in >> fileName;
            ui->serverStatusLabel->setText("����" + fileName + "...");
            bytesReceived += fileNameSize;
            localFile = new QFile(fileName);
            if(!localFile->open(QFile::WriteOnly))
            {

                qDebug() << "open file error!";
                return;
            }
        } else{

           if(flagDown == false)
            {
                if(wfileName.isEmpty()) return ;
                QString down;
                in >> down;
                if(down == "~")
                {
                    ui->serverStatusLabel->setText("�����ļ�");
                    flagDown = true;
                    on_sendpushButton_clicked();
                    bytesReceived = 0;
                    return ;
                }
            }

           return;
       }
    }

    if(bytesReceived < totalBytes)
    {
        bytesReceived += tcpServerConnection->bytesAvailable();
        inBlock = tcpServerConnection->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
    }

    ui->serverProgressBar->setMaximum(totalBytes);
    ui->serverProgressBar->setValue(bytesReceived);
    if(bytesReceived == totalBytes)
    {
        localFile->close();
        ui->serverStatusLabel->setText("�����ļ�" + fileName + "���");
        totalBytes = 0;
        bytesReceived = 0;
        fileNameSize = 0;
        flagDown = false;
    }    
}

void Widget::displayError(QAbstractSocket::SocketError)
{
    qDebug() << tcpServerConnection->errorString();
    tcpServerConnection->close();
    ui->serverProgressBar->reset();
    ui->serverStatusLabel->setText("����˾���");
    ui->startButton->setEnabled(true);
}

void Widget::on_startButton_clicked()
{
    start();
}


void Widget::on_addFileButton_clicked()
{
    wfileName = QFileDialog::getOpenFileName(this);
    if(!wfileName.isEmpty())
    {
        ui->addFileButton->setEnabled(false);

        ui->serverStatusLabel->setText("���ļ��ɹ�");
    }
}

void Widget::on_sendpushButton_clicked()
{
    if(!wfileName.isEmpty())
    {
        ui->sendpushButton->setEnabled(false);

        localFile = new QFile(wfileName);
        if(!localFile->open(QFile::ReadOnly))
        {
           qDebug() << "open file error!";
           return;
        }
        totalBytes = localFile->size();
        QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
        sendOut.setVersion(QDataStream::Qt_5_11);
        QString currentFileName = wfileName.right(wfileName.size() - wfileName.lastIndexOf('/')-1);
        sendOut << qint64(0) << qint64(0) << currentFileName;
        totalBytes += outBlock.size();
        sendOut.device()->seek(0);
        sendOut<<totalBytes<<qint64((outBlock.size() - sizeof(qint64)*2));
        bytesToWrite = totalBytes - tcpServerConnection->write(outBlock);
    }
}
