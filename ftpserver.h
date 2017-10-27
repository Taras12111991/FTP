#ifndef FTPSERVER_H
#define FTPSERVER_H

#include <QWidget>
#include <QDirModel>
#include <QLineEdit>
#include "uftpd.h"


class FtpServer : public QWidget
{
    Q_OBJECT
public:
    FtpServer(QWidget *parent = 0);

signals:
    void serverStopped();

private slots:
    void startServer();
    void stopServer();
    void setName(const QString &name);
    void setPassword(const QString &password);
    void setFtpPort(const QString &ftpPort);
    void setTftpPort(const QString &tftpPort);
    void setPath(const QString &path);
    void showCurrentPath(QModelIndex index);

private:
    int runServer(int ftpPort, int tftpPort);

private:
    uev_ctx_t *ctx;
    QString _name;
    QString _password;
    QString _path;
    int _ftpPort;
    int _tftpPort;
    QDirModel *_model;
    QLineEdit *_editPath;

    QThread *_uiThread;
    QThread *_serverThread;

};

#endif // FTPSERVER_H
