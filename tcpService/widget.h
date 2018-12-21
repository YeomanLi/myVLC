#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtNetwork>
#include <QString>

namespace Ui {
    class Widget;
}

class Widget : public QWidget {
    Q_OBJECT
public:
    Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Widget *ui;
    QTcpServer tcpServer;
    QTcpSocket *tcpServerConnection;
    qint64 totalBytes; //����ܴ�С��Ϣ
    qint64 bytesReceived; //���յ����ݵĴ�С
    qint64 fileNameSize; //�ļ����Ĵ�С��Ϣ
    QString fileName; //����ļ���
    QFile *localFile; //�����ļ�
    QByteArray inBlock; //���ݻ�����
    
    QString wfileName;
    qint64 bytesWritten; //�Ѿ��������ݴ�С
    qint64 bytesToWrite; //ʣ�����ݴ�С
    qint64 loadSize; //ÿ�η������ݵĴ�С
    QByteArray outBlock; //���ݻ������������ÿ��Ҫ���͵�����

    bool flagDown;      //����

private slots:
    void on_sendpushButton_clicked();
    void on_addFileButton_clicked();
    void on_startButton_clicked();
    void start(); //��ʼ����
    void acceptConnection(); //��������
    void updateServerProgress(); //���½���������������
    void displayError(QAbstractSocket::SocketError socketError);

    void updateClientProgress(qint64); //�������ݣ����½�����
};

#endif // WIDGET_H
