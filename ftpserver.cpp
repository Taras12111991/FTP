#include "ftpserver.h"
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeView>
#include <QLabel>
#include <QDebug>
#include <QThread>

FtpServer::FtpServer(QWidget *parent):
    QWidget(parent), _name("username"), _password(""), _path("/home"),
    _model(0), _editPath(0), _ftpPort(0), _tftpPort(0), _serverThread(0)
{
    ctx = new uev_ctx_t;

    _serverThread = new QThread(this);
    connect(_serverThread, SIGNAL(started()), this, SLOT(startServer()), Qt::DirectConnection);
    connect(this, SIGNAL(serverStopped()), _serverThread, SLOT(quit()), Qt::DirectConnection);

    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    QHBoxLayout *editLayout = new QHBoxLayout(this);
    QHBoxLayout *pathLayout = new QHBoxLayout(this);
    QVBoxLayout *vLayout = new QVBoxLayout(this);

    QPushButton *start = new QPushButton("Start Server", this);
    //connect(start, SIGNAL(clicked()), this, SLOT(startServer()));
    connect(start, SIGNAL(clicked()), _serverThread, SLOT(start()));
    QPushButton *stop = new QPushButton("Stop Server", this);
    connect(stop, SIGNAL(clicked()), this, SLOT(stopServer()));

    buttonLayout->addWidget(start);
    buttonLayout->addWidget(stop);

    QWidget *buttonWgt = new QWidget(this);
    buttonWgt->setLayout(buttonLayout);

    QLabel *name = new QLabel("Username", this);
    QLineEdit *editName = new QLineEdit(this);
    editName->setText(_name);
    connect(editName, SIGNAL(textChanged(QString)), this, SLOT(setName(QString)));

    QLabel *password = new QLabel("Password", this);
    QLineEdit *editPassword = new QLineEdit(this);
    editPassword->setEchoMode(QLineEdit::Password);
    connect(editPassword, SIGNAL(textChanged(QString)), this, SLOT(setPassword(QString)));

    QLabel *ftpPort = new QLabel("FTP Port",this);
    QLineEdit *editFtpPort = new QLineEdit(this);
    QIntValidator *ftpValidator = new QIntValidator(editFtpPort);
    editFtpPort->setValidator(ftpValidator);
    connect(editFtpPort, SIGNAL(textChanged(QString)), this, SLOT(setFtpPort(QString)));

    QLabel *tftpPort = new QLabel("TFTP Port",this);
    QLineEdit *editTftpPort = new QLineEdit(this);
    QIntValidator *tftpValidator = new QIntValidator(editTftpPort);
    editTftpPort->setValidator(tftpValidator);
    connect(editTftpPort, SIGNAL(textChanged(QString)), this, SLOT(setTftpPort(QString)));

    editLayout->addWidget(name);
    editLayout->addWidget(editName);
    editLayout->addWidget(password);
    editLayout->addWidget(editPassword);
    editLayout->addWidget(ftpPort);
    editLayout->addWidget(editFtpPort);
    editLayout->addWidget(tftpPort);
    editLayout->addWidget(editTftpPort);

    QLabel *path = new QLabel("Path" , this);
    _editPath = new QLineEdit(this);
    connect(_editPath, SIGNAL(textChanged(QString)), this, SLOT(setPath(QString)));

    pathLayout->addWidget(path);
    pathLayout->addWidget(_editPath);

    _model = new QDirModel(this);
    QTreeView *treeView = new QTreeView(this);
    connect(treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(showCurrentPath(QModelIndex)));

    treeView->setModel(_model);

    vLayout->addLayout(editLayout);
    vLayout->addLayout(pathLayout);
    vLayout->addWidget(treeView);
    vLayout->addWidget(buttonWgt);

    setLayout(vLayout);
}

void FtpServer::setName(const QString &name)
{
    _name = name;
}

void FtpServer::setPassword(const QString &password)
{
    _password = password;
}

void FtpServer::setFtpPort(const QString &ftpPort)
{
    _ftpPort = ftpPort.toInt();
}

void FtpServer::showCurrentPath(QModelIndex index)
{
    _editPath->setText(_model->filePath(index));
}

void FtpServer::setPath(const QString &path)
{
    _path = path;
}

void FtpServer::startServer()
{
    runServer(_ftpPort, _tftpPort);
}

int FtpServer::runServer(int ftpPort, int tftpPort)
{
    //uev_ctx_t ctx;

    background = 0;
    do_syslog--;

    do_ftp = ftpPort;
    do_tftp = tftpPort;

    home = const_cast<char*>(_path.toStdString().c_str());
    if (pw)
        pw->pw_name = const_cast<char*>(_name.toStdString().c_str());

    if (string_compare(prognm, "in.tftpd")) {
        inetd      = 1;
        do_ftp     = 0;
        do_tftp    = 1;
        background = 0;
        do_syslog  = 1;
    } else if (string_compare(prognm, "in.ftpd")) {
        inetd      = 1;
        do_ftp     = 1;
        do_tftp    = 0;
        background = 0;
        do_syslog  = 1;
    }

    if (do_syslog) {
        openlog(prognm, LOG_PID | LOG_NDELAY, LOG_FTP);
        setlogmask(LOG_UPTO(loglevel));
    }

    qDebug() << "Initializing ...";
    if (init(ctx)) {
        ERR(0, "Failed initializing, exiting.");
        return 1;
    }

    if (inetd) {
        int sd;
        pid_t pid;

        qDebug() << QString("Started from inetd, serving files from %0 ...").arg(home);

        /* Ensure socket is non-blocking */
        sd = STDIN_FILENO;
        (void)fcntl(sd, F_SETFL, fcntl(sd, F_GETFL, 0) | O_NONBLOCK);

        if (do_tftp)
            pid = tftp_session(ctx, sd);
        else
            pid = ftp_session(ctx, sd);

        if (-1 == pid)
            return 1;
        return 0;
    }

    if (background) {
        qDebug() << "Daemonizing ...";
        if (-1 == daemon(0, 0)) {
            ERR(errno, "Failed daemonizing");
            return 1;
        }
    }

    qDebug() << QString("Serving files as PID %1 ...").arg(getpid());
    return serve_files(ctx);
}

void FtpServer::stopServer()
{
    do_ftp = 0;
    do_tftp = 0;
}


void FtpServer::setTftpPort(const QString &tftpPort)
{
    _tftpPort = tftpPort.toInt();
}


